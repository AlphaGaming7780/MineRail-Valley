#pragma once

#include <SFML/Graphics.hpp>

struct RenderCommand
{
    enum class Type { Sprite, Shape, Text };

    Type type = Type::Sprite;

    // Données minimales
    sf::Texture* texture = nullptr;
    sf::Color color = sf::Color::White;

    sf::Vector2f position;
    sf::Vector2f origin;
    sf::Vector2f size;
    sf::Vector2f scale;
    sf::Angle    rotation = sf::Angle::Zero;

    int zIndex = 0;

    // Animation (optionnel)
    bool animated = false;
    int frameX = 0;
    int frameY = 0;
    int frameWidth = 0;
    int frameHeight = 0;
};