#pragma once

#include <Game/AssetDatabase/AssetDatabase.h>
#include<Game/AssetDatabase/AssetType/MapAsset.h>
#include<Game/AssetDatabase/TileDatabase.h>
#include<glaze/glaze.hpp>

namespace Game
{
	class MapDatabase : public AssetDatabase<MapAsset>
	{
	public:

		static MapDatabase& Instance()
		{
			static MapDatabase inst;
			return inst;
		}

		MapAsset* Load_Impl(const std::string& path) override
		{
            std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

            MapData* data = new MapData();
            auto read_error = glz::read_file_json(data, fullPath, std::string{});

            return _BuildFromData(*data);
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Maps") / "DEFAULT.json").string();
		}

        MapAsset* _BuildFromData(const MapData& data)
        {
            MapAsset* asset = new MapAsset();
            asset->MapSize = data.MapSize;
            asset->Difficulty = data.Difficulty;

            for (int i = 0; i < data.tiles.size(); i++)
            {
                std::string tilePath = data.tiles[i];
                TileAsset* tile = TileDatabase::Instance().Load(tilePath);
                asset->tiles.push_back(tile);
            }

            return asset;
        }

		void Unload(const std::string& path, bool force = false)
		{

			if (!m_Assets.contains(path))
				return;

			if (!DecrementRefCount(path, force))
				return;

			MapAsset* map = m_Assets[path];

			for (int i = 0; i < map->tiles.size(); i++)
			{
				TileDatabase::Instance().Unload(map->tiles[i]);
			}


			delete m_Assets[path];
			m_Assets.erase(path);
			m_RefCount.erase(path);

		}

    private:
        MapDatabase() : AssetDatabase("MapDatabase") {}
    };
}