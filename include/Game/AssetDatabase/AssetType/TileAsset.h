#pragma once
#include<Game/AssetDatabase.h>
#include<string>

namespace Game
{
	struct TileData
	{
		std::string TexturePath;
		bool canBuild = false;
	};

	struct TileAsset
	{
		sf::Texture* TextureAsset = nullptr;
		bool CanBuild = false;
	};
}