#pragma once

#include <SFML/Graphics.hpp>
#include <Game/AssetDatabase/AssetType/StationData.h>
#include <Game/GameObjects/Tracks/TrackObject.h>

namespace Game
{
	class StationObject : public TrackObjectBase
	{
		friend class World;
	public:
		std::string m_StationColor;
		sf::Vector2i m_AllowedTileConnection;
		


	protected:
		StationObject() : TrackObjectBase() {}
		StationObject(const StationData& stationData) : TrackObjectBase(stationData.object)
		{
			m_StationColor = stationData.m_SationColor;
		}
	};
}