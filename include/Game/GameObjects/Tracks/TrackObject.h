#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TrackObject : public GameObject
	{
	public:
		// Représente la connection avce les autres tracks
		TrackObject* m_First;
		TrackObject* m_Second;

		std::string m_StraightTexturePath;
		std::string m_BendTexturePath;

		sf::Texture* m_StraightTexture = nullptr;
		sf::Texture* m_bendTexture = nullptr;

		TileObject* m_Tile = nullptr;

		void Update();
		void OnDestroy();

		void SetTile(TileObject* tile);


	protected:
		TrackObject() : GameObject() {}
		TrackObject(const TrackData& trackData) : GameObject(trackData.object)
		{
			m_StraightTexturePath = trackData.m_StraightTrackTexture;
			m_BendTexturePath = trackData.m_BendTrackTexture;
		}

	};
}