#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
//#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TileObject;
	class TrackObject : public GameObject
	{
		friend class World;
	public:

		// Représente la connection avce les autres tracks
		TrackObject* m_First = nullptr;
		TrackObject* m_Second = nullptr;

		std::string m_StraightTexturePath;
		std::string m_BendTexturePath;

		sf::Texture* m_StraightTexture = nullptr;
		sf::Texture* m_BendTexture = nullptr;

		TileObject* m_Tile = nullptr;

		void Update();
		void OnDestroy();

		void SetTile(TileObject* tile);

		void UpdateSprite();

		void ResolveConnections();

		std::vector<TrackObject*> GetAdjacentTracks() const;

	protected:
		TrackObject() : GameObject() {}
		TrackObject(const TrackData& trackData) : GameObject(trackData.object)
		{
			m_StraightTexturePath = trackData.m_StraightTrackTexture;
			m_BendTexturePath = trackData.m_BendTrackTexture;

			m_StraightTexture = TextureDatabase::Instance().Load(m_StraightTexturePath);
			m_BendTexture = TextureDatabase::Instance().Load(m_BendTexturePath);

		}
	};
}