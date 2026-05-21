#pragma once
#include <Game/GameObjects/GameObject.h>
#include <Game/AssetDatabase/AssetType/TileData.h>
#include <Game/GameObjects/Tracks/TrackObjectBase.h>

namespace Game
{
	class TileObject : public GameObject
	{
		friend class World;
	public:

		virtual ~TileObject()
		{
			if(m_PlacedTrack) m_PlacedTrack->m_Tile = nullptr;
		}

		sf::Vector2i m_Index = sf::Vector2i(-1, -1);
		bool m_CanBuild = false;

		TileObject* m_Up = nullptr;
		TileObject* m_Left = nullptr;
		TileObject* m_Right = nullptr;
		TileObject* m_Down = nullptr;

		TrackObjectBase* m_PlacedTrack = nullptr;

	protected:
		TileObject() : GameObject() {}
		TileObject(const TileData& tileData, const sf::Vector2i& index) : GameObject(tileData.object) 
		{
			m_Index = index;
			m_CanBuild = tileData.m_CanBuild;

			float x = m_Index.x * m_Size.x;
			float y = m_Index.y * m_Size.y;

			m_Sprite->setPosition(sf::Vector2f(x, y));
			m_Enabled = m_Animated;
		}
	};
}