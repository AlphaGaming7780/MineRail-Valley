#pragma once

#include <SFML/Graphics.hpp>
#include <Game/AssetDatabase/AssetDatabase.h>

namespace Game
{
	class TextureDatabase : public AssetDatabase<sf::Texture>
	{
	public:
		sf::Texture* Load_Impl(const std::string& path) override
		{
			sf::Texture* texture = new sf::Texture();
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();
			if (!texture->loadFromFile(fullPath)) {
				m_Logger.ErrorO("[TextureAsset] Failed to load texture: ", fullPath);
				delete texture;
				texture = nullptr;
			}
			return texture;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("PallasEngine") / "DEFAULT.png").string();
		}
	};
}