#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "UIAnchor.hpp"

namespace pallas
{
    class UIElement
    {
    public:
        virtual ~UIElement();

        virtual void Draw(sf::RenderTexture& canvas);
        virtual void UpdateLayout(const sf::View& view);

        // Gestion hiérarchie
        void AddChild(UIElement* child);
        void RemoveChild(UIElement* child);
        UIElement* GetParent() const { return m_Parent; }

        // Position
        sf::Vector2f GetAbsolutePosition() const { return m_AbsPosition; }
        sf::Vector2f GetRelativePosition() const { return m_RelPosition; }
        sf::Vector2f GetSize() const { return m_Size; }

        void SetRelativePosition(sf::Vector2f pos) { m_RelPosition = pos; }
        void SetSize(sf::Vector2f size) { m_Size = size; }
        void SetAnchor(UIAnchor anchor) { m_Anchor = anchor; }

        virtual bool HitTest(const sf::Vector2f& mousePos) const;

        bool HandleMouseEvent(const sf::Vector2f& mousePos, bool mousePressed);

        virtual void OnFocusGain();
        virtual void OnFocusLost();
        virtual void OnMouseEnter() {}
        virtual void OnMouseLeave() {}
        virtual void OnMouseHover() {}
        virtual void OnMouseDown();
        virtual void OnMouseUp();
        virtual void OnClick();

        void ReDraw();

    protected:
        UIElement* m_Parent = nullptr;
        std::vector<UIElement*> m_Children;

        UIAnchor m_Anchor = UIAnchor::TopLeft;
        sf::Vector2f m_RelPosition = { 0, 0 };
        sf::Vector2f m_AbsPosition = { 0, 0 };
        sf::Vector2f m_Size = { 0, 0 };

        bool m_IsHovered = false;
        bool m_WasPressedInside = false;
        bool m_PreviousPressed = false;
        bool m_HasFocus = false;

        void ComputeAbsolutePosition(const sf::View& view);
    };
}