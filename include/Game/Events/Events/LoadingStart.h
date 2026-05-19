#pragma once

#include <Game/GameStates.h>
#include <Game/AssetDatabase/AssetType/MapAsset.h>

namespace Game
{
	struct LoadingStart
	{
		Purpose m_Purpose;
		GameMode m_GameMode;
		MapData* m_MapData;

		LoadingStart() = default;

		LoadingStart(Purpose purpose, GameMode gameMode)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
			m_MapData = nullptr;
		}

		LoadingStart(Purpose purpose, GameMode gameMode, MapData* mapData)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
			m_MapData = mapData;
		}
	};
}