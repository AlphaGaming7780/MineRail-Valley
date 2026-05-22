#include <Game/UI/Widgets/UIButton.hpp>

#include <Game/AssetDatabase.h>
#include <Game/UI/UIManager.hpp>
#include <Game/AudioManager.hpp>

namespace Game
{
    UIButton::UIButton(
        sf::Vector2f position,
        sf::Vector2f size,
        UIAnchor anchor,
        sf::Texture* normalTexture,
        sf::Texture* hoveredTexture,
        sf::Texture* clickedTexture
    )
        : UIWidget(position, size, anchor)
        , m_TextureNormal(normalTexture)
        , m_TextureHover(hoveredTexture)
        , m_TextureClicked(clickedTexture)
    {
        // Texture par défaut
        m_Sprite = new sf::Sprite(*m_TextureNormal);
    }

    UIButton::~UIButton()
    {
        delete m_Sprite;
    }

    void UIButton::UpdateLayout(const sf::View& view)
    {
        UIWidget::UpdateLayout(view);

        // Met à jour la position du sprite
        m_Sprite->setPosition(m_AbsPosition);

        // Met à jour la taille (si tu veux du scale)
        m_Sprite->setScale({
                m_Size.x / static_cast<float>(m_Sprite->getTexture().getSize().x),
                m_Size.y / static_cast<float>(m_Sprite->getTexture().getSize().y)
            }
        );
    }

    void UIButton::Draw(sf::RenderTexture& canvas)
    {
        canvas.draw(*m_Sprite);

        // Dessine les enfants
        UIWidget::Draw(canvas);
    }

    void UIButton::OnMouseEnter()
    {
        if (!m_TextureHover) return;

        m_Sprite->setTexture(*m_TextureHover);
        ReDraw();
    }

    void UIButton::OnMouseLeave()
    {
        m_IsPressed = false;

        if (!m_TextureNormal) return;

        m_Sprite->setTexture(*m_TextureNormal);
        ReDraw();
    }

    void UIButton::OnMouseDown()
    {
        m_IsPressed = true;

        if (!m_TextureClicked) return;

        m_Sprite->setTexture(*m_TextureClicked);
        ReDraw();
    }

    void UIButton::OnMouseUp()
    {
        if (m_IsPressed)
        {
            m_IsPressed = false;

            if (!m_TextureHover) return;

            m_Sprite->setTexture(*m_TextureHover);
            ReDraw();
        }
    }

    void UIButton::OnClick()
    {
        // Auto-play the menu click SFX on every button click for consistent
        // feedback across all screens. Volume 80% so it doesn't drown out music.
        AudioManager::Instance().PlaySound("Sounds\\Menu Selection Click.wav", 0.8f);

        if (m_Callback)
        {
            m_Callback();
        }
    }

    void UIButton::SetCallback(std::function<void()> callback)
    {
        m_Callback = callback;
    }
}



