#pragma once
#include<vector>


namespace Game
{
	struct MapData
	{
		int MapSize = 0;
		int Difficulty = 0;
		std::vector<std::string> tiles;
	};
}