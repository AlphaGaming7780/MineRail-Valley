#pragma once
#include <SFML/Graphics.hpp>
#include <Game/UI/UIWidget.hpp>

namespace Game
{
    // UIImage: static textured rectangle. Useful for logos, decorative panels,
    // album covers in the music player, biome thumbnails in the level select, etc.
    //
    // Note: the texture is NOT owned by this widget. Load it via TextureDatabase
    // from the screen that creates the UIImage, and Unload it in that screen's
    // destructor (same pattern as UIButton / UILabel).
    class UIImage : public UIWidget
    {
    public:
        UIImage(
            sf::Texture* texture = nullptr,
            sf::Vector2f position = { 0, 0 },
            sf::Vector2f size = { 0, 0 },
            UIAnchor anchor = UIAnchor::TopLeft,
            sf::Color tint = sf::Color::White
        );

        virtual ~UIImage();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Swap the displayed texture at runtime (e.g. when changing tracks).
        void SetTexture(sf::Texture* texture);

        // Multiplicative tint applied to the sprite. Default = white (no tint).
        void SetTint(const sf::Color& color);

        // Default HitTest from UIElement (rect-based) is fine. UIImage has no
        // mouse interaction so we don't override any On*() events.

    private:
        sf::Texture* m_Texture = nullptr;
        sf::Sprite*  m_Sprite  = nullptr;
        sf::Color    m_Tint    = sf::Color::White;
    };
}
