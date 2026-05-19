#pragma once

#include <SFML/Audio.hpp>
#include <Game/AssetDatabase/AssetDatabase.h>

namespace Game
{
	class SoundDatabase : public AssetDatabase<sf::SoundBuffer>
	{
	public:

		static SoundDatabase& Instance()
		{
			static SoundDatabase inst;
			return inst;
		}

		sf::SoundBuffer* Load_Impl(const std::string& path) override
		{
			sf::SoundBuffer* soundBuffer = new sf::SoundBuffer();
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();
			if (!soundBuffer->loadFromFile(fullPath))
			{
				m_Logger.ErrorO("[SoundDatabase] Failed to load sound effect: ", fullPath);
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