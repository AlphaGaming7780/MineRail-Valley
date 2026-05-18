#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace pallas
{
    struct CameraComponent
    {
        sf::Vector2f position = { 0,0 };
        float        zoom = 1.f;
        sf::Angle    rotation = sf::Angle::Zero;
        sf::Vector2f size = { 0.f, 0.f };
        sf::Vector2f baseSize = { 0.f, 0.f };
        sf::FloatRect globalBounds = { {0,0}, {0,0} };
        sf::FloatRect localBounds = { {0,0}, {0,0} };
    };
}
