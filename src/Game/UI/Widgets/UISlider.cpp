#include <Game/UI/Widgets/UISlider.hpp>
#include <Game/Events/InputManager.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>

namespace Game
{
    UISlider::UISlider(sf::Texture* trackTex, sf::Texture* handleTex,
                       sf::Vector2f position, sf::Vector2f size, UIAnchor anchor,
                       float minValue, float maxValue, float initialValue)
        : m_TrackTex(trackTex)
        , m_HandleTex(handleTex)
        , m_Min(minValue)
        , m_Max(maxValue)
        , m_Value(std::clamp(initialValue, minValue, maxValue))
    {
        m_RelPosition = position;
        m_Size = size;
        m_Anchor = anchor;

        if (m_TrackTex)  m_TrackSprite  = new sf::Sprite(*m_TrackTex);
        if (m_HandleTex) m_HandleSprite = new sf::Sprite(*m_HandleTex);
    }

    UISlider::~UISlider()
    {
        delete m_TrackSprite;
        delete m_HandleSprite;
        m_TrackSprite = nullptr;
        m_HandleSprite = nullptr;
    }

    float UISlider::ValueToX() const
    {
        float range = m_Max - m_Min;
        if (range <= 0.f) return m_AbsPosition.x;
        float t = (m_Value - m_Min) / range;
        return m_AbsPosition.x + t * m_Size.x;
    }

    float UISlider::XToValue(float x) const
    {
        float range = m_Max - m_Min;
        if (m_Size.x <= 0.f || range <= 0.f) return m_Min;
        float t = (x - m_AbsPosition.x) / m_Size.x;
        return std::clamp(m_Min + t * range, m_Min, m_Max);
    }

    void UISlider::UpdateLayout(const sf::View& view)
    {
        ComputeAbsolutePosition(view);

        if (m_TrackSprite && m_TrackTex)
        {
            sf::Vector2u t = m_TrackTex->getSize();
            if (t.x > 0 && t.y > 0)
                m_TrackSprite->setScale({ m_Size.x / float(t.x), m_Size.y / float(t.y) });
            m_TrackSprite->setPosition(m_AbsPosition);
        }

        if (m_HandleSprite && m_HandleTex)
        {
            sf::Vector2u t = m_HandleTex->getSize();
            // Render the handle at the size given by its native ratio, scaled to the
            // slider height. Width is proportional to height to keep the knob square.
            float handleH = m_Size.y;
            float handleW = (t.y > 0) ? handleH * (float(t.x) / float(t.y)) : handleH;
            if (t.x > 0 && t.y > 0)
                m_HandleSprite->setScale({ handleW / float(t.x), handleH / float(t.y) });
            m_HandleSprite->setPosition({ ValueToX() - handleW * 0.5f, m_AbsPosition.y });
        }

        UIElement::UpdateLayout(view);
    }

    void UISlider::Draw(sf::RenderTexture& canvas)
    {
        if (m_TrackSprite)  canvas.draw(*m_TrackSprite);
        if (m_HandleSprite) canvas.draw(*m_HandleSprite);
        UIElement::Draw(canvas);
    }

    void UISlider::OnMouseDown()
    {
        // Snap immediately to click X. The UIManager passes the current UI mouse
        // pos into HandleMouseEvent, which sets m_IsHovered and then calls
        // OnMouseDown. We can't read the mouse position from here directly, so
        // we just flag the drag — Update() will resync from the mouse position
        // every frame as long as the button stays pressed.
        m_IsDragging = true;
        // Snap immediately too (without waiting next frame).
        sf::Vector2f mp = InputManager::Instance().GetMouseUIPos();
        OnMouseDragTo(mp);
    }

    void UISlider::OnMouseUp()
    {
        if (m_IsDragging && m_OnCommit) m_OnCommit(m_Value);
        m_IsDragging = false;
    }

    void UISlider::Update()
    {
        if (m_IsDragging)
        {
            // Catch a mouse release that happened outside the slider's hitbox
            // — without this, the drag would stay sticky after releasing far
            // from the widget.
            const bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if (!pressed)
            {
                if (m_OnCommit) m_OnCommit(m_Value);
                m_IsDragging = false;
            }
            else
            {
                sf::Vector2f mp = InputManager::Instance().GetMouseUIPos();
                OnMouseDragTo(mp);
            }
        }
        UIElement::Update();
    }

    void UISlider::OnMouseDragTo(const sf::Vector2f& mouseUI)
    {
        if (!m_IsDragging) return;
        float newValue = XToValue(mouseUI.x);
        if (newValue != m_Value)
        {
            m_Value = newValue;
            if (m_OnChange) m_OnChange(m_Value);
            ReDraw();
        }
    }

    void UISlider::SetValue(float v)
    {
        v = std::clamp(v, m_Min, m_Max);
        if (v == m_Value) return;
        m_Value = v;
        if (m_OnChange) m_OnChange(m_Value);
        ReDraw();
    }

    void UISlider::SetRange(float minV, float maxV)
    {
        m_Min = minV;
        m_Max = std::max(maxV, minV);
        SetValue(m_Value);  // re-clamp
    }
}
