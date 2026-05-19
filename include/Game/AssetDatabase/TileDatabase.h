#pragma once

#include<glaze/glaze.hpp>

#include<Game/AssetDatabase/AssetDatabase.h>
#include<Game/AssetDatabase/AssetType/TileData.h>
#include<Game/AssetDatabase/TextureDatabase.h>

namespace Game
{
	class TileDatabase : public AssetDatabase<TileData>
	{
	public:

		static TileDatabase& Instance()
		{
			static TileDatabase inst;
			return inst;
		}

		TileData* Load_Impl(const std::string& path) override
		{
			const std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

			TileData* data = new TileData();
			glz::error_ctx read_error = glz::read_file_json(data, fullPath, std::string{});

			if (read_error)
			{
				m_Logger.ErrorO("Failed to load the JSON: ", read_error.custom_error_message);
				delete data;
				return nullptr;
			}

			return data;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Tiles") / "DEFAULT.json").string();
		}

	private:
		TileDatabase() : AssetDatabase("TileDatabase") {}

	};
}