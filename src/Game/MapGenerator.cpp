#include <Game/MapGenerator.hpp>

namespace Game
{
	MapGenerator::~MapGenerator()
	{

	}
	MapGenerator::Map MapGenerator::GenerateWorld()
	{
		const int worldSize = 128;
		
		Map map;

		for (int x = 0; x < worldSize; x++)
		{
			for (int y = 0; y < worldSize; y++)
			{
				int elevation = GetElevation(GenType::Flat, x, y);
				Tile t = { .x = x, .y = y, .elevation = elevation };
				map.tiles.push_back(t);
			}
		}

		return map;

	}
	int MapGenerator::GetElevation(GenType genType, int x, int y)
	{
		switch (genType)
		{
		case GenType::Flat:
		default:
			return 1;

		}
	}
}
