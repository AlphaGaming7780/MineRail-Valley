#pragma once

#include <Game//GameStates.h>
#include <Game/AssetDatabase/AssetType/MapData.h>

namespace Game
{
	struct LoadingComplete
	{
		Purpose m_Purpose;
		GameMode m_GameMode;
		MapData* m_MapData;

		LoadingComplete() = default;

		LoadingComplete(Purpose purpose, GameMode gameMode, MapData* mapData = nullptr)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
			m_MapData = mapData;
		}
	};
}
