#pragma once

#include <SFML/Graphics.hpp>
#include <PallasEngine/Logging.hpp>

#include <Game/UI/UIGroup.hpp>
#include <Game/Events/EventManager.hpp>

namespace Game
{
    class UIManager
    {
    public:
        static UIManager& Instance();

        //void UpdateLayout(sf::RenderWindow& window);
        void UpdateLayout();
        void Update(sf::RenderWindow& window);
        void Draw(sf::RenderWindow& window);

        template<typename TGroup>
        TGroup& SetRoot();
        void ResetRoot();

        void ReDraw();

        sf::Vector2f MapPixelToCoords(sf::Vector2i screenPos) const;
        //sf::Vector2i GetMousePosInUIView(sf::Vector2i screenPos) const;
        bool IsMouseOverUI() const;
        bool IsMouseOverUI(sf::Vector2i screenPos) const;
        bool IsMouseOverUI(sf::Vector2f uiMousePos) const;

        

        pallas::Logger m_Logger = pallas::Logger("UIManager");

    private:
        UIManager();
        ~UIManager();

        UIGroup* m_Root = nullptr;
        sf::View m_UIView;
        sf::RenderTexture m_UICanvas;

        std::vector<UIElement*> m_InvalidatedElements;

        bool HasInvalidatedParent(UIElement* element);
        bool IsAlreadyInvalidated(UIElement* element);
        bool ShouldAddInvalidation(UIElement* element);

        friend class UIElement;
        void ReDrawUIElement(UIElement* element);

        void FlushRedraw();

        void ResizeView(const sf::Event::Resized& ev);
        
    };

    template<typename TGroup>
    TGroup& UIManager::SetRoot()
    {
        static_assert(std::is_base_of_v<UIGroup, TGroup>, "TGroup must derive from UIGroup");

        //TGroup* e = new TGroup();
        //m_Root.AddChild(e);
        //e->UpdateLayout(m_UIView);
        //e->ReDraw();

        // Remove all sub element that needed a redraw since we are going to delete the root of them.
        m_InvalidatedElements.clear();

        delete m_Root;
        m_Root = nullptr;
        m_Root = new TGroup();
        m_Root->UpdateLayout(m_UIView);
        m_Root->ReDraw();
        return *static_cast<TGroup*>(m_Root);
    }
}
