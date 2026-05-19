#pragma once
#include <Game/UI/UIElement.hpp>

namespace Game
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