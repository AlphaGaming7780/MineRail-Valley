#pragma once

#include <Game/GameStates.h>
#include <Game/AssetDatabase/AssetType/MapAsset.h>

namespace Game
{
	struct LoadingStart
	{
		Purpose m_Purpose;
		GameMode m_GameMode;
		MapAsset* m_MapAsset;

		LoadingStart() = default;

		LoadingStart(Purpose purpose, GameMode gameMode)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
			m_MapAsset = nullptr;
		}

		LoadingStart(Purpose purpose, GameMode gameMode, MapAsset* mapAsset)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
			m_MapAsset = mapAsset;
		}
	};
}