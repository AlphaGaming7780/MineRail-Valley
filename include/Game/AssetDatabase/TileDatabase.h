#pragma once

#include<glaze/glaze.hpp>

#include<Game/AssetDatabase/AssetDatabase.h>
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
			const std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

			TileData data;
			glz::error_ctx read_error = glz::read_file_json(data, fullPath, std::string{});

			if (read_error)
			{
				m_Logger.ErrorO("Failed to load the JSON: ", read_error.custom_error_message);
				return nullptr;
			}

			return _BuildFromData(data);
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Tiles") / "DEFAULT.json").string();
		}

		TileAsset* _BuildFromData(const TileData& data)
		{
			TileAsset* asset = new TileAsset();
			asset->CanBuild = data.canBuild;
			asset->TextureAsset = TextureDatabase::Instance().Load(data.TexturePath);
			return asset;
		}

		void Unload_Impl(const std::string& path, bool force = false) override
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