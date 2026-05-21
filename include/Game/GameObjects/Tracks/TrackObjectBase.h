#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TileObject;
	class TrackObjectBase : public GameObject
	{
		friend class World;
	public:

		TrackObjectBase* m_First = nullptr;
		TrackObjectBase* m_Second = nullptr;

		TileObject* m_Tile = nullptr;

		void SetTile(TileObject* tile);
		virtual void UpdateSprite() {};
		virtual void ResolveConnections() {};

		virtual void OnDestroy() override;

		std::vector<TrackObjectBase*> GetAdjacentTracks() const;

	protected:
		TrackObjectBase() : GameObject() { m_RenderLayer = RenderLayer::Tracks; }
		TrackObjectBase(const ObjectData& obData) : GameObject(obData)
		{
			m_RenderLayer = RenderLayer::Tracks;
		}
	};
}