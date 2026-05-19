#pragma once

#include <Game/GameStates.h>

namespace Game
{
	struct LoadingStart
	{
		Purpose m_Purpose;
		GameMode m_GameMode;

		LoadingStart() = default;

		LoadingStart(Purpose purpose, GameMode gameMode)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
		}
	};
}