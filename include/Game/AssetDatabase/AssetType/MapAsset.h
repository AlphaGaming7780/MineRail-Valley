#pragma once
#include<Game/AssetDatabase/AssetType/TileAsset.h>
#include<vector>


namespace Game
{
	struct MapData
	{
		int MapSize = 0;
		int Difficulty = 0;
		std::vector<std::string> tiles;
	};

	struct MapAsset
	{
		int MapSize = 0;
		int Difficulty = 0;
		std::vector<TileAsset*> tiles;
	};
}