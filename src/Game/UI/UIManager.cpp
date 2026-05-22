#include <Game/UI/UIManager.hpp>
#include <Game/UI/UIGroups/UIRoot.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/Events/InputManager.hpp>

namespace Game
{
    UIManager& UIManager::Instance()
    {
        static UIManager instance;
        return instance;
    }

    UIManager::UIManager()
    {
        m_Logger.InfoO("Init UIManager.");

        EventManager::Instance().Register<sf::Event::Resized>(this);

        m_UIView.setSize({1920, 1080}); //(sf::Vector2f)sf::VideoMode::getDesktopMode().size
        m_UIView.setCenter(m_UIView.getSize() / 2.f);

		// Resize une première fois pour adapter la view à la taille de la fenêtre, et éviter les problèmes de redimensionnement à l'ouverture.
		ResizeView({ .size = GameWindow::Instance().GetSize()});

        // UI par default transparent.
        m_Root = new UIRoot();

        UpdateLayout();
    }

    UIManager::~UIManager() 
    {
        m_InvalidatedElements.clear();
        delete m_Root;
        m_Root = nullptr;
        m_Logger.Info("UIManager destroyed.");
    }

    void UIManager::OnEvent(const sf::Event::Resized& ev)
    {
        ResizeView(ev); 
        UpdateLayout();
    }

    bool UIManager::HasInvalidatedParent(UIElement* element)
    {
        UIElement* p = element->GetParent();

        while (p != nullptr)
        {
            for (auto* e : m_InvalidatedElements)
            {
                if (e == p)
                    return true;
            }
            p = p->GetParent();
        }

        return false;

    }

    bool UIManager::IsAlreadyInvalidated(UIElement* element)
    {
        for (auto* e : m_InvalidatedElements)
        {
            if (e == element)
                return true;
        }
        return false;

    }

    bool UIManager::ShouldAddInvalidation(UIElement* element)
    {
        for (UIElement* e : m_InvalidatedElements)
        {
            // 1. L'élément est déjà dans la liste
            if (e == element)
                return false;

            // 2. Un parent est déjà dans la liste
            UIElement* p = element->GetParent();
            while (p != nullptr)
            {
                if (p == e)
                    return false;
                p = p->GetParent();
            }

            // 3. Un enfant est déjà dans la liste → on doit remplacer l'enfant par le parent
            UIElement* c = e->GetParent();
            while (c != nullptr)
            {
                if (c == element)
                {
                    // On retire l'enfant, car le parent va le redessiner
                    m_InvalidatedElements.erase(
                        std::remove(m_InvalidatedElements.begin(), m_InvalidatedElements.end(), e),
                        m_InvalidatedElements.end()
                    );
                    break;
                }
                c = c->GetParent();
            }
        }

        return true;
    }


    void UIManager::ReDrawUIElement(UIElement* element)
    {
        if (ShouldAddInvalidation(element))
            m_InvalidatedElements.push_back(element);
    }

    void UIManager::FlushRedraw()
    {
        if (m_InvalidatedElements.empty())
            return;

        // Redessiner chaque élément invalidé
        for (auto* element : m_InvalidatedElements)
        {
            element->UpdateLayout(m_UIView); // Pas sur de ca, mais bon, c'est pas tjs utile de mettre a jour le layout quand on redraw (genre couleur qui change)
            element->Draw(m_UICanvas);
        }

        // Finaliser
        m_UICanvas.display();

        // Nettoyer la liste
        m_InvalidatedElements.clear();
    }

    void UIManager::ResizeView(const sf::Event::Resized& ev)
    {
        sf::Vector2u screenSize = { 1920, 1080 }; // sf::VideoMode::getDesktopMode().size;
        float windowWidth = static_cast<float>(ev.size.x);
        float windowHeight = static_cast<float>(ev.size.y);

        float windowRatio = windowWidth / windowHeight;
        float targetRatio = (float)screenSize.x / screenSize.y;

        m_UIView.setCenter((sf::Vector2f)screenSize / 2.f);

        // Si la fenêtre est plus large → élargir horizontalement
        if (windowRatio > targetRatio)
        {
            float newWidth = screenSize.y * windowRatio;
            m_UIView.setSize({ newWidth, static_cast<float>(screenSize.y) });
        }
        else
        {
            // Si la fenêtre est plus haute → élargir verticalement
            float newHeight = screenSize.x / windowRatio;
            m_UIView.setSize({ static_cast<float>(screenSize.x), newHeight });
        }

        // Toujours remplir toute la fenêtre
        m_UIView.setViewport({ {0.f, 0.f,}, { 1.f, 1.f } });
    }

    //void UIManager::UpdateLayout(sf::RenderWindow& window)
    void UIManager::UpdateLayout()
    {
        m_UICanvas.setView(m_UIView);
        if(!m_UICanvas.resize((sf::Vector2u)m_UIView.getSize()))
            m_Logger.ErrorO("Failed to resize the RenderTexture to: ", m_UIView.getSize());

        ReDraw();
    }

    void UIManager::Update(sf::RenderWindow& window)
    {
 
        if (m_PendingRootSetter)
        {
            m_PendingRootSetter();
            m_PendingRootSetter = nullptr;
        }

        if (!window.hasFocus()) return;

        m_InUpdate = true;

        sf::Vector2f mousePos = InputManager::Instance().GetMouseUIPos();

        m_Logger.InfoO("Mouse pos: ", mousePos, ", Ui view sizze: ", m_UIView.getSize());

        bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if(m_Root) 
            m_Root->HandleMouseEvent(mousePos, mousePressed);

        m_InUpdate = false;
    }

    void UIManager::Draw(sf::RenderWindow& window)
    {
#ifdef DEBUG
        auto perf = pallas::PerformanceLogger::Scoped("UIManager.Draw()");
#endif // DEBUG
        FlushRedraw();
        sf::Sprite s = sf::Sprite(m_UICanvas.getTexture());
        s.setOrigin(s.getLocalBounds().size / 2.f);
        s.setPosition(m_UIView.getCenter());
        //s.setPosition(m_UIView.getCenter() - m_UIView.getSize() / 2.f);
        window.setView(m_UIView);
        window.draw(s);
    }

    void UIManager::ResetRoot()
    {
        SetRoot<UIRoot>();
    }

    void UIManager::ReDraw()
    {
		ReDrawUIElement(m_Root);
    }

    sf::Vector2f UIManager::MapPixelToCoords(sf::Vector2i screenPos) const
    {
        sf::Vector2f coords = GameWindow::Instance().Get().mapPixelToCoords(screenPos, m_UIView);

        // Correction du letterboxing vertical
        float viewH = m_UIView.getSize().y;
        float uiH = 1080.f; // ta résolution logique

        if (viewH > uiH)
        {
            float offsetY = (viewH - uiH) * 0.5f;
            coords.y -= offsetY;
        }

        // Correction du letterboxing horizontal (si jamais)
        float viewW = m_UIView.getSize().x;
        float uiW = 1920.f;

        if (viewW > uiW)
        {
            float offsetX = (viewW - uiW) * 0.5f;
            coords.x -= offsetX;
        }

        return coords;
    }

    //sf::Vector2i UIManager::GetMousePosInUIView(sf::Vector2i screenPos) const
    //{
    //    sf::Vector2i mp = screenPos;
    //    sf::Vector2u ws = GameWindow::Instance().GetSize();
    //    sf::Vector2f us = m_UIView.getSize();

    //    sf::Vector2i mousePos(
    //        int((float(mp.x) / ws.x) * us.x),
    //        int((float(mp.y) / ws.y) * us.y)
    //    );

    //    return mousePos;

    //}

    bool UIManager::IsMouseOverUI() const
    {
        return m_Root&& m_Root->HitTest(InputManager::Instance().GetMouseUIPos());
    }

    bool UIManager::IsMouseOverUI(sf::Vector2i screenPos) const
    {
        return m_Root && m_Root->HitTest(MapPixelToCoords(screenPos));
    }
    bool UIManager::IsMouseOverUI(sf::Vector2f uiMousePos) const
    {
        return m_Root && m_Root->HitTest(uiMousePos);
    }
}



