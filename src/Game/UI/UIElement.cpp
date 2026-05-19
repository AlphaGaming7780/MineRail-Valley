#include <Game/UI/UIElement.hpp>
#include <Game/UI/UIManager.hpp>

namespace Game
{
    void UIElement::AddChild(UIElement* child)
    {
        child->m_Parent = this;
        m_Children.push_back(child);
    }

    void UIElement::RemoveChild(UIElement* child)
    {
        if (!child)
            return;

        auto it = std::find(m_Children.begin(), m_Children.end(), child);
        if (it != m_Children.end())
        {
            // On enlève le parent
            child->m_Parent = nullptr;

            // On retire du vecteur
            m_Children.erase(it);
        }
    }

    bool UIElement::HitTest(const sf::Vector2f& mousePos) const
    {
        sf::FloatRect rect(m_AbsPosition, m_Size);
        return rect.contains(mousePos);
    }

    bool UIElement::HandleMouseEvent(const sf::Vector2f& mousePos, bool mousePressed)
    {
        bool contains = HitTest(mousePos);

        // 1. Propagation aux enfants
        bool result = false;
        for (auto& child : m_Children)
        {
            if (child->HandleMouseEvent(mousePos, mousePressed))
            {
                if (m_IsHovered && !contains)
                {
                    m_IsHovered = false;
                    OnMouseLeave();
                }

                if (m_HasFocus && mousePressed)
                {
                    m_HasFocus = false;
                    OnFocusLost();
                }
                result = true;
            }
        }

        // Faire ca, signifie qu'on met a jour tous les élement au lieu de seulement propager l'event au élement ou la souris est dessus.
        // Mais j'en ai besoin pour la gestion du Focus, le focus sera utile pour les text input
        if (result) return true;

        // 2. Gestion hover
        if (contains)
        {
            if (!m_IsHovered)
            {
                m_IsHovered = true;
                OnMouseEnter();
            }

            OnMouseHover();
        }
        else
        {
            if (m_IsHovered)
            {
                m_IsHovered = false;
                OnMouseLeave();
            }
        }

        // 3. Gestion MouseDown / MouseUp / Click
        if (contains)
        {
            // MouseDown
            if (mousePressed && !m_PreviousPressed)
            {
                m_WasPressedInside = true;
                OnMouseDown();
            }

            // MouseUp
            if (!mousePressed && m_PreviousPressed)
            {
                OnMouseUp();

                if (m_WasPressedInside) 
                {
                    OnClick(); // Click = Down + Up dans l'élément
                    if (!m_HasFocus)
                    {
                        m_HasFocus = true;
                        OnFocusGain();
                    }
                }

                m_WasPressedInside = false;
            }

            m_PreviousPressed = mousePressed;
            return true;
        }
        else
        {
            
            if (mousePressed && m_HasFocus) 
            {
                m_HasFocus = false;
                OnFocusLost();
            }
            else 
            {
                // Si on relâche en dehors, pas de click
                m_WasPressedInside = false;
            }

            m_PreviousPressed = mousePressed;
            return false;
        }
        return false; // just in case;
    }

    void UIElement::OnFocusGain()
    {
        { UIManager::Instance().m_Logger.InfoO("OnFocusGain: ", typeid(*this).name()); }
    }

    void UIElement::OnFocusLost()
    {
        { UIManager::Instance().m_Logger.InfoO("OnFocusLost: ", typeid(*this).name()); }
    }

    void UIElement::OnMouseDown()
    {
        if(m_Parent)
            m_Parent->OnMouseDown();
    }

    void UIElement::OnMouseUp()
    {
        if(m_Parent)
            m_Parent->OnMouseUp();
    }

    void UIElement::OnClick()
    {
        if(m_Parent)
            m_Parent->OnClick();
    }

    void UIElement::ReDraw()
    {
        // Se check est utile pour les texture qui on de la transparence, car sinon elle s'additionne au lieu de se remplacer. y a tjs un problème si le parent a aussi une texture transparente
		// Mais dans le cas d'element simple, genre texture sans transparence, c'est pas utile et cela repprésente une perte de performance, car cela oblige a redessiner le parent et tout les éléments enfant de ce parent.
        // TODO: Il faudrais donc trouver un moyen de différencier les éléments qui ont besoin de ce check et ceux qui n'en ont pas besoin, pour éviter de redessiner des éléments inutilement.
        if(m_Parent)
			UIManager::Instance().ReDrawUIElement(m_Parent);
		else
            UIManager::Instance().ReDrawUIElement(this);
    }

    void UIElement::ComputeAbsolutePosition(const sf::View& view)
    {
        sf::Vector2f parentPos = { 0,0 }; // view.getCenter() - view.getSize() / 2.f;
        sf::Vector2f parentSize = view.getSize();

        if (m_Parent)
        {
            parentPos = m_Parent->m_AbsPosition;
            parentSize = m_Parent->m_Size;
        }

        sf::Vector2f pos = m_RelPosition;

        switch (m_Anchor)
        {
        case UIAnchor::TopLeft:
            break;

        case UIAnchor::TopRight:
            pos.x = parentSize.x - m_Size.x - m_RelPosition.x;
            break;

        case UIAnchor::BottomLeft:
            pos.y = parentSize.y - m_Size.y - m_RelPosition.y;
            break;

        case UIAnchor::BottomRight:
            pos.x = parentSize.x - m_Size.x - m_RelPosition.x;
            pos.y = parentSize.y - m_Size.y - m_RelPosition.y;
            break;

        case UIAnchor::Center:
            pos.x = (parentSize.x - m_Size.x) / 2.f + m_RelPosition.x;
            pos.y = (parentSize.y - m_Size.y) / 2.f + m_RelPosition.y;
            break;
        }

        m_AbsPosition = parentPos + pos;
    }

    UIElement::~UIElement()
    {
        for (auto& child : m_Children)
        {
            delete child;
        }

        if (m_Parent)
            m_Parent->RemoveChild(this);
    }

    void UIElement::Draw(sf::RenderTexture& canvas)
    {
        for (auto& child : m_Children)
            child->Draw(canvas);
    }

    void UIElement::UpdateLayout(const sf::View& view)
    {
        ComputeAbsolutePosition(view);

        for (auto& child : m_Children)
            child->UpdateLayout(view);
    }
}