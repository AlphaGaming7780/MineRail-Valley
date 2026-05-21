#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TileObject;
	class TrackObject;
	class TrackObjectBase : public GameObject
	{
		friend class World;
	public:

		TrackObjectBase* m_First = nullptr;
		TrackObjectBase* m_Second = nullptr;

		TileObject* m_Tile = nullptr;

		virtual void SetTile(TileObject* tile);
		virtual void UpdateSprite() {};
		virtual void ResolveConnections();

		void Update() override;
		void OnDestroy() override;

		void DisconnectTrack();
		void ConnectTrack(TrackObjectBase* a, TrackObjectBase* b);

		std::vector<TrackObjectBase*> GetAdjacentTracks() const;
		std::vector<TrackObject*> GetAdjacentTrackObjects() const;

		bool m_CanBeDeleted = true;

	protected:
		TrackObjectBase() : GameObject() { m_RenderLayer = RenderLayer::Tracks; }
		TrackObjectBase(const ObjectData& obData) : GameObject(obData)
		{
			m_RenderLayer = RenderLayer::Tracks;
		}
			
	};
}