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
			sf::Music* music = new sf::Music();
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();
			if (!music->openFromFile(fullPath))
			{
				m_Logger.ErrorO("[MusicDatabase] Failed to load music: ", fullPath);
				delete music;
				music = nullptr;
			}
			return music;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Music") / "DEFAULT.mp3").string();
		}
	};
}