#pragma once

#include <SFML/Graphics.hpp>
#include <Game/UI/UIWidget.hpp>
#include <Game/AssetDatabase/FontDatabase.h>

namespace Game
{
    class UILabel : public UIWidget
    {
    public:
        UILabel(
            const std::string& text,
            sf::Vector2f position = { 0,0 },
            UIAnchor anchor = UIAnchor::TopLeft,
            sf::Font* fontAsset = nullptr,
            unsigned int characterSize = 20,
            sf::Color textColor = { 207, 182, 151 }
        );

        virtual ~UILabel();

        // Layout + Draw
        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Text API
        void SetText(const std::string& text);
        void SetColor(const sf::Color& color);
        void SetCharacterSize(unsigned int size);

    private:
        // Asset ref
        sf::Font* m_FontAsset = nullptr;

        // Loaded font
        sf::Font* m_Font = nullptr;

        // SFML text
        sf::Text* m_Text = nullptr;

        // Internal helper
        sf::Font* LoadFont(sf::Font** asset, const char* fallbackPath);
    };
}