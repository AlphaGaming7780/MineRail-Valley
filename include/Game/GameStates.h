#pragma once

namespace Game
{
	enum GameState
	{
		Booting,
		Terminated,
		//BootingPrefabs,
		//WorldDisposed,
		WorldReady,
		Quitting,
		Loading
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