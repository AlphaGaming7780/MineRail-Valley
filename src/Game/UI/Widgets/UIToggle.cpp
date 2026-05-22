#include <Game/UI/Widgets/UIToggle.hpp>

namespace Game
{
    UIToggle::UIToggle(sf::Vector2f position, sf::Vector2f size, UIAnchor anchor,
                       sf::Texture* offNormal, sf::Texture* offHover,
                       sf::Texture* onNormal,  sf::Texture* onHover)
        : m_TexOffNormal(offNormal)
        , m_TexOffHover(offHover)
        , m_TexOnNormal(onNormal)
        , m_TexOnHover(onHover)
    {
        m_RelPosition = position;
        m_Size = size;
        m_Anchor = anchor;
        RefreshSprite();
    }

    UIToggle::~UIToggle()
    {
        delete m_Sprite;
        m_Sprite = nullptr;
    }

    void UIToggle::RefreshSprite()
    {
        sf::Texture* tex = nullptr;
        if (m_Checked) tex = m_IsHovered ? m_TexOnHover  : m_TexOnNormal;
        else            tex = m_IsHovered ? m_TexOffHover : m_TexOffNormal;
        if (tex == nullptr) return;

        if (m_Sprite == nullptr) m_Sprite = new sf::Sprite(*tex);
        else                      m_Sprite->setTexture(*tex, true);
        ReDraw();
    }

    void UIToggle::UpdateLayout(const sf::View& view)
    {
        UIElement::UpdateLayout(view);
        if (m_Sprite != nullptr && m_Size.x > 0.f && m_Size.y > 0.f)
        {
            sf::Vector2u t = m_Sprite->getTexture().getSize();
            if (t.x > 0 && t.y > 0)
                m_Sprite->setScale({ m_Size.x / float(t.x), m_Size.y / float(t.y) });
            m_Sprite->setPosition(m_AbsPosition);
        }
    }

    void UIToggle::Draw(sf::RenderTexture& canvas)
    {
        if (m_Sprite != nullptr)
            canvas.draw(*m_Sprite);
        UIElement::Draw(canvas);
    }

    void UIToggle::OnMouseEnter() { RefreshSprite(); }
    void UIToggle::OnMouseLeave() { RefreshSprite(); }

    void UIToggle::OnClick()
    {
        m_Checked = !m_Checked;
        RefreshSprite();
        if (m_Callback) m_Callback(m_Checked);
    }

    void UIToggle::SetChecked(bool checked)
    {
        if (m_Checked == checked) return;
        m_Checked = checked;
        RefreshSprite();
    }
}
