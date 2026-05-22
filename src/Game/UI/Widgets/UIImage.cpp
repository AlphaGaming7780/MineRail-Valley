#include <Game/UI/Widgets/UIImage.hpp>

namespace Game
{
    UIImage::UIImage(sf::Texture* texture, sf::Vector2f position, sf::Vector2f size,
                     UIAnchor anchor, sf::Color tint)
    {
        m_RelPosition = position;
        m_Size = size;
        m_Anchor = anchor;
        m_Texture = texture;
        m_Tint = tint;

        if (m_Texture != nullptr)
        {
            m_Sprite = new sf::Sprite(*m_Texture);
            m_Sprite->setColor(m_Tint);
        }
    }

    UIImage::~UIImage()
    {
        delete m_Sprite;
        m_Sprite = nullptr;
    }

    void UIImage::UpdateLayout(const sf::View& view)
    {
        ComputeAbsolutePosition(view);
        if (m_Sprite != nullptr && m_Texture != nullptr && m_Size.x > 0.f && m_Size.y > 0.f)
        {
            const sf::Vector2u texSize = m_Texture->getSize();
            if (texSize.x > 0 && texSize.y > 0)
            {
                m_Sprite->setScale({
                    m_Size.x / static_cast<float>(texSize.x),
                    m_Size.y / static_cast<float>(texSize.y)
                });
            }
            m_Sprite->setPosition(m_AbsPosition);
        }
        UIElement::UpdateLayout(view);
    }

    void UIImage::Draw(sf::RenderTexture& canvas)
    {
        if (m_Sprite != nullptr)
            canvas.draw(*m_Sprite);
        UIElement::Draw(canvas);
    }

    void UIImage::SetTexture(sf::Texture* texture)
    {
        m_Texture = texture;
        delete m_Sprite;
        m_Sprite = nullptr;
        if (m_Texture != nullptr)
        {
            m_Sprite = new sf::Sprite(*m_Texture);
            m_Sprite->setColor(m_Tint);
        }
        ReDraw();
    }

    void UIImage::SetTint(const sf::Color& color)
    {
        m_Tint = color;
        if (m_Sprite != nullptr)
            m_Sprite->setColor(m_Tint);
        ReDraw();
    }
}
