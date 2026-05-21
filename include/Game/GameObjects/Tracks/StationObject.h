#pragma once

#include <SFML/Graphics.hpp>
#include <Game/AssetDatabase/AssetType/StationData.h>
#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/GameObjects/Tracks/TrackObject.h>

namespace Game
{
	class StationObject : public TrackObjectBase
	{
		friend class World;
	public:
		sf::Color m_StationColor;
		sf::Vector2i m_AllowedTileConnection;
		
		void SetTile(TileObject* tile) override
		{
			TrackObjectBase::SetTile(tile);
			tile->m_CanBuild = false;
		}

	protected:
		StationObject() : TrackObjectBase() { m_RenderLayer = RenderLayer::Stations; }
		StationObject(const StationData& stationData) : TrackObjectBase(stationData.object)
		{
			m_StationColor = stationData.m_SationColor;
			m_RenderLayer = RenderLayer::Stations;
		}
	};
}