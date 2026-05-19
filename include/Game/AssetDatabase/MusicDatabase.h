#pragma once

#include <SFML/Audio.hpp>
#include <Game/AssetDatabase/AssetDatabase.h>

namespace Game
{
	class MusicDatabase : public AssetDatabase<sf::Music>
	{
	public:
		sf::Music* Load_Impl(const std::string& path) override
		{

		}
	};
}