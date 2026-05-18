#pragma once

namespace Game
{
	enum GameState
	{
		Booting,
		Terminated,
		Loading,
		WorldReady,
		Quitting
	};

	enum class GameMode
	{
		None,
		Other,
		MainMenu,
		InGame,
	};

	enum class Purpose
	{
		None,
		Other,
		NewGame,
		SaveGame,
		LoadGame,
		Cleanup, //JSP trop quoi, mais just in case
	};
}