#pragma once

#include <Game/UI/UIElement.hpp>

namespace Game
{
    class UIWidget : public UIElement
    {
    public:
        UIWidget(
            sf::Vector2f position = { 0,0 },
            sf::Vector2f size = { 0,0 },
            UIAnchor anchor = UIAnchor::TopLeft
        )
        {
            m_RelPosition = position;
            m_Anchor = anchor;
            m_Size = size;
        }

        virtual ~UIWidget() = default;

        void UpdateSpriteSize(sf::Sprite* sprite)
        {
            if (sprite != nullptr && m_Size.x > 0.f && m_Size.y > 0.f)
            {
                const sf::Vector2u texSize = sprite->getTexture().getSize();
                if (texSize.x > 0 && texSize.y > 0)
                {
                    sprite->setScale({
                        m_Size.x / static_cast<float>(texSize.x),
                        m_Size.y / static_cast<float>(texSize.y)
                        });
                }
            }
        }
    };
}
