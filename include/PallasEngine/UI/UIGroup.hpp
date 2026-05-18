#pragma once
#include <PallasEngine/UI/UIElement.hpp>

namespace pallas
{
    class UIGroup : public UIElement
    {
    public:
        bool HitTest(const sf::Vector2f& mousePos) const override {
            for (auto* child : m_Children)
                if (child->HitTest(mousePos))
                    return true;
            return false;
        }
    };
}