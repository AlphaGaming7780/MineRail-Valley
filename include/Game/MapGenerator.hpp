#pragma once

#include <vector>

namespace Game
{
	class MapGenerator
	{
	public:

		struct Tile
		{
			int x, y;
			int elevation;
			// BiomeAsset
		};

		struct Map
		{
			std::vector<Tile> tiles;
		};

		enum GenType
		{
			Flat,
			FastNoise,
			FastNoise2
		};

		
		~MapGenerator();

		MapGenerator& Instance()
		{
			static MapGenerator inst;
			return inst;
		}

		Map GenerateWorld();

	private:
		MapGenerator();

		int GetElevation(GenType genType, int x, int y);

	};
}