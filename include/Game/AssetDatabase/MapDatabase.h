#pragma once

#include <Game/AssetDatabase/AssetDatabase.h>
#include<Game/AssetDatabase/AssetType/MapData.h>
#include<Game/AssetDatabase/TileDatabase.h>
#include<glaze/glaze.hpp>

namespace Game
{
	class MapDatabase : public AssetDatabase<MapData>
	{
	public:

		static MapDatabase& Instance()
		{
			static MapDatabase inst;
			return inst;
		}

		MapData* Load_Impl(const std::string& path) override
		{
            std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

			MapData* data = new MapData();
			glz::error_ctx read_error = glz::read_file_json(*data, fullPath, std::string{});

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
			return (std::filesystem::path("Maps") / "DEFAULT.json").string();
		}

    private:
        MapDatabase() : AssetDatabase("MapDatabase") {}
    };
}