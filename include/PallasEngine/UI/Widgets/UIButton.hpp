#pragma once

#include <PallasEngine/AssetDatabase/AssetTypes/TextureAsset.hpp>

#include <PallasEngine/UI/UIWidget.hpp>
#include <functional> //pour le std::function.

namespace pallas
{
    class UIButton : public UIWidget
    {
    public:
        UIButton(
            sf::Vector2f position = {0,0},
            sf::Vector2f size = {0,0},
            UIAnchor anchor = UIAnchor::TopLeft,
            TextureAsset* normalTexture = nullptr,
            TextureAsset* hoveredTexture = nullptr,
            TextureAsset* clickedTexture = nullptr
        );

        virtual ~UIButton();

        // Layout + Draw
        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Events
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void OnMouseDown() override;
        void OnMouseUp() override;
        void OnClick() override;

        // Callback
        void SetCallback(std::function<void()> callback);

    private:
        // Asset refs
        TextureAsset* m_TextureAssetNormal = nullptr;
        TextureAsset* m_TextureAssetHover = nullptr;
        TextureAsset* m_TextureAssetClicked = nullptr;

        // Loaded textures
        sf::Texture* m_TextureNormal = nullptr;
        sf::Texture* m_TextureHover = nullptr;
        sf::Texture* m_TextureClicked = nullptr;

        // Sprite
        sf::Sprite* m_Sprite = nullptr;

        // State
        bool m_IsPressed = false;

        // Callback
        std::function<void()> m_Callback;

        // Internal helper
        sf::Texture* LoadTexture(TextureAsset** asset, const char* fallbackPath);
    };
}

