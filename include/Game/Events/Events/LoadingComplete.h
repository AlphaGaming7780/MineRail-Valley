#pragma once

#include <Game//GameStates.h>

namespace Game
{
	struct LoadingComplete
	{
		Purpose m_Purpose;
		GameMode m_GameMode;

		LoadingComplete() = default;

		LoadingComplete(Purpose purpose, GameMode gameMode)
		{
			m_Purpose = purpose;
			m_GameMode = gameMode;
		}
	};
}