#pragma once

#include <Game/GameObjects/Tracks/TrackObjectBase.h>
#include <Game/AssetDatabase/AssetType/TrackData.h>

namespace Game
{
	class TrackObject : public TrackObjectBase
	{
		friend class World;
	public:

		std::string m_StraightTexturePath;
		std::string m_BendTexturePath;

		sf::Texture* m_StraightTexture = nullptr;
		sf::Texture* m_BendTexture = nullptr;

		void Update() override;
		void OnDestroy() override;

		void UpdateSprite() override;
		void ResolveConnections() override;

	protected:
		TrackObject() : TrackObjectBase() {}
		TrackObject(const TrackData& trackData) : TrackObjectBase(trackData.object)
		{
			m_StraightTexturePath = trackData.m_StraightTrackTexture;
			m_BendTexturePath = trackData.m_BendTrackTexture;

			m_StraightTexture = TextureDatabase::Instance().Load(m_StraightTexturePath);
			m_BendTexture = TextureDatabase::Instance().Load(m_BendTexturePath);
		}
	};
}