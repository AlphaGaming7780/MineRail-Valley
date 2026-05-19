#pragma once

#include <Game/AssetDatabase/AssetDatabase.h>
#include<Game/AssetDatabase/AssetType/TileAsset.h>
#include<Game/AssetDatabase/TextureDatabase.h>

namespace Game
{
	class TileDatabase : public AssetDatabase<TileAsset>
	{
	public:

		static TileDatabase& Instance()
		{
			static TileDatabase inst;
			return inst;
		}

		TileAsset* Load_Impl(const std::string& path) override
		{
			std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

			TileData* data = new TileData();
			auto read_error = glz::read_file_json(data, fullPath, std::string{});

			return _BuildFromData(*data);
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Tile") / "DEFAULT.json").string();
		}

		TileAsset* _BuildFromData(const TileData& data)
		{
			TileAsset* asset = new TileAsset();
			asset->CanBuild = data.canBuild;
			asset->TextureAsset = TextureDatabase::Instance().Load(data.TexturePath);
			return asset;
		}

		void Unload(const std::string& path, bool force = false) 
		{

			if (!m_Assets.contains(path))
				return;

			if (!DecrementRefCount(path, force))
				return;

			TextureDatabase::Instance().Unload(m_Assets[path]->TextureAsset);


			delete m_Assets[path];
			m_Assets.erase(path);
			m_RefCount.erase(path);

		}
	private:
		TileDatabase() : AssetDatabase("TileDatabase") {}

	};
}