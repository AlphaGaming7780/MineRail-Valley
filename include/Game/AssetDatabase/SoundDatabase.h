#pragma once

#include <SFML/Audio.hpp>
#include <Game/AssetDatabase/AssetDatabase.h>

namespace Game
{
	class MusicDatabase : public AssetDatabase<sf::SoundBuffer>
	{
	public:
		sf::SoundBuffer* Load_Impl(const std::string& path) override
		{
			sf::SoundBuffer* soundBuffer = new sf::SoundBuffer();
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();
			if (!soundBuffer->loadFromFile(fullPath))
			{
				m_Logger.ErrorO("[MusicDatabase] Failed to load texture: ", fullPath);
				delete soundBuffer;
				soundBuffer = nullptr;
			}
			return soundBuffer;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Sound") / "DEFAULT.mp3").string();
		}
	};
}