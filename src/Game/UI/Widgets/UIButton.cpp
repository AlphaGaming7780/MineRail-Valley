#include <PallasEngine/UI/Widgets/UIButton.hpp>

#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>
#include <PallasEngine/UI/UIManager.hpp>

namespace pallas
{
    UIButton::UIButton(
        sf::Vector2f position,
        sf::Vector2f size,
        UIAnchor anchor,
        TextureAsset* normalTexture,
        TextureAsset* hoveredTexture,
        TextureAsset* clickedTexture
    )
        : UIWidget(position, size, anchor),
        m_TextureAssetNormal(normalTexture),
        m_TextureAssetHover(hoveredTexture),
        m_TextureAssetClicked(clickedTexture)
    {
        // Charge les textures (ou fallback)
        m_TextureNormal = LoadTexture(&m_TextureAssetNormal, "UI\\Button.png");
        m_TextureHover = LoadTexture(&m_TextureAssetHover, "UI\\Button_hover.png");
        m_TextureClicked = LoadTexture(&m_TextureAssetClicked, "UI\\Button_clicked.png");

        // Texture par défaut
        m_Sprite = new sf::Sprite(*m_TextureNormal);
    }

    UIButton::~UIButton()
    {
        delete m_Sprite;
        if (m_TextureAssetNormal)  m_TextureAssetNormal->Unload();
        if (m_TextureAssetHover)   m_TextureAssetHover->Unload();
        if (m_TextureAssetClicked) m_TextureAssetClicked->Unload();
    }

    sf::Texture* UIButton::LoadTexture(TextureAsset** asset, const char* fallbackPath)
    {
        if (*asset == nullptr)
        {
            if (!AssetDatabase::Instance().TryGetAsset(fallbackPath, *asset))
            {
                (*asset) = TextureAsset::GetDefault();
            }
        }

        return (*asset)->LoadObject();
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



