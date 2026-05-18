#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>

namespace pallas
{
    struct Transform
    {
        sf::Vector2f m_Position = { 0.f, 0.f };
        sf::Vector2f m_Origin = { 0.f, 0.f };
        sf::Vector2f m_Size = { 1.f, 1.f };
        sf::Vector2f m_Scale = { 1.f, 1.f };
        sf::Angle    m_Rotation = sf::Angle::Zero;
    };
}
