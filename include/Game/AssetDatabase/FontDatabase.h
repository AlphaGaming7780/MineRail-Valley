#pragma once

#include <SFML/Graphics.hpp>
#include <Game/AssetDatabase/AssetDatabase.h>

namespace Game
{
	class FontDatabase : public AssetDatabase<sf::Font>
	{
	public:

		static FontDatabase& Instance()
		{
			static FontDatabase inst;
			return inst;
		}

		sf::Font* Load_Impl(const std::string& path) override
		{
			sf::Font* font = new sf::Font();
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();
			if (!font->openFromFile(fullPath))
			{
				m_Logger.ErrorO("[FontDatabase] Failed to load texture: ", fullPath);
				delete font;
				font = nullptr;
			}
			return font;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Fonts") / "DEFAULT.png").string();
		}
	};
}