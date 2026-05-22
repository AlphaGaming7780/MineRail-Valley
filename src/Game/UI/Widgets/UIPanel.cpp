#include <Game/UI/Widgets/UIPanel.hpp>

namespace Game
{
    UIPanel::UIPanel(sf::Texture* background, sf::Vector2f position, sf::Vector2f size,
                     UIAnchor anchor, sf::Color color)
    {
        m_RelPosition = position;
        m_Size = size;
        m_Anchor = anchor;
        m_Color = color;
        m_Texture = background;

        if (m_Texture != nullptr)
        {
            m_Sprite = new sf::Sprite(*m_Texture);
        }
        m_Rect.setFillColor(m_Color);
    }

    UIPanel::~UIPanel()
    {
        delete m_Sprite;
        m_Sprite = nullptr;
    }

    void UIPanel::UpdateLayout(const sf::View& view)
    {
        ComputeAbsolutePosition(view);

        if (m_Sprite != nullptr && m_Texture != nullptr && m_Size.x > 0.f && m_Size.y > 0.f)
        {
            const sf::Vector2u t = m_Texture->getSize();
            if (t.x > 0 && t.y > 0)
            {
                m_Sprite->setScale({ m_Size.x / static_cast<float>(t.x),
                                     m_Size.y / static_cast<float>(t.y) });
            }
            m_Sprite->setPosition(m_AbsPosition);
        }
        else
        {
            m_Rect.setSize(m_Size);
            m_Rect.setPosition(m_AbsPosition);
        }
        UIElement::UpdateLayout(view);
    }

    void UIPanel::Draw(sf::RenderTexture& canvas)
    {
        if (m_Sprite != nullptr)
            canvas.draw(*m_Sprite);
        else
            canvas.draw(m_Rect);

        UIElement::Draw(canvas);
    }

    void UIPanel::SetTexture(sf::Texture* texture)
    {
        m_Texture = texture;
        delete m_Sprite;
        m_Sprite = nullptr;
        if (m_Texture != nullptr)
            m_Sprite = new sf::Sprite(*m_Texture);
        ReDraw();
    }

    void UIPanel::SetColor(const sf::Color& color)
    {
        m_Color = color;
        m_Rect.setFillColor(color);
        ReDraw();
    }

    bool UIPanel::HitTest(const sf::Vector2f& mousePos) const
    {
        if (!m_BlocksMouse)
            return false;
        return UIElement::HitTest(mousePos);
    }
}
