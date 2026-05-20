#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
	struct ObjectData
	{
		std::string m_Texture = "Textures\\DEFAULT.png";
		sf::Color m_Color = sf::Color::White;
		sf::Vector2f m_Size = { 1.f, 1.f };


		bool m_Animated = false;
		int m_AnimFramesX = 1;       // nombre de frames en X
		int m_AnimFramesY = 1;       // nombre de frames en 
		float m_AnimDuration = 0.f;
	};
}

namespace glz
{
    template<>
    struct meta<sf::Vector2f>
    {
        using T = sf::Vector2f;
        static constexpr auto value = object(
            "x", &T::x,
            "y", &T::y
        );
    };

    template<>
    struct meta<sf::Color>
    {
        using T = sf::Color;
        static constexpr auto value = object(
            "r", &T::r,
            "g", &T::g,
            "b", &T::b,
            "a", &T::a
        );
    };

    template<>
    struct meta<Game::ObjectData>
    {
        using T = Game::ObjectData;
        static constexpr auto value = object(
            "texture", &T::m_Texture,
            "color", &T::m_Color,
            "size", &T::m_Size,
            "animated", &T::m_Animated,
            "framesX", &T::m_AnimFramesX,
            "framesY", &T::m_AnimFramesY,
            "duration", &T::m_AnimDuration
        );
    };
}