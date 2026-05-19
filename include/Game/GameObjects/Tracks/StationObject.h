#pragma once

#include <SFML/Graphics.hpp>
#include <Game/GameObjects/Tracks/TrackObject.h>

namespace Game
{
	class StationObject : public TrackObject
	{
	public:
		sf::Color m_Color;
	};
}