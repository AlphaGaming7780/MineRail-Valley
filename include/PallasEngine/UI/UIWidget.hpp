#pragma once

#include <PallasEngine/UI/UIElement.hpp>

namespace pallas
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
    };
}
