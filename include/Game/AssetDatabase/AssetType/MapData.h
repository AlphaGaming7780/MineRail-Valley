#pragma once
#include<vector>
#include<string>


namespace Game
{
	struct MapData
	{
		int MapSize = 0;
		int Difficulty = 0;
		std::string BgAudioPath;
		std::vector<std::string> tiles;
	};
}