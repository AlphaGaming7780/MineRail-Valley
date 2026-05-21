#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
//#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TileObject;
	class TrackObjectBase : public GameObject
	{
		friend class World;
	public:

		TrackObject* m_First = nullptr;
		TrackObject* m_Second = nullptr;

		TileObject* m_Tile = nullptr;

		virtual void SetTile(TileObject* tile);
		virtual void UpdateSprite() = 0;
		virtual void ResolveConnections() = 0;

		std::vector<TrackObjectBase*> GetAdjacentTracks() const;

	protected:
		TrackObjectBase() : GameObject() {}
		TrackObjectBase(const ObjectData& obData) : GameObject(obData)
		{}
	};
}