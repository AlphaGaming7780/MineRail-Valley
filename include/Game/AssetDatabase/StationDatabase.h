#pragma once

#include<glaze/glaze.hpp>
#include<magic_enum/magic_enum.hpp>

#include<Game/AssetDatabase/AssetDatabase.h>
#include<Game/AssetDatabase/AssetType/StationData.h>
#include<Game/AssetDatabase/TextureDatabase.h>

namespace Game
{
	class StationDatabase : public AssetDatabase<StationData>
	{
	public:

		static StationDatabase& Instance()
		{
			static StationDatabase inst;
			return inst;
		}

		StationData* Load_Impl(const std::string& path) override
		{
			const std::string fullPath = (std::filesystem::path(DatabasePath) / path).string();

			StationData* data = new StationData();
			glz::error_ctx read_error = glz::read_file_json(data, fullPath, std::string{});

			if (read_error)
			{
				m_Logger.ErrorO("Failed to load the JSON: ", magic_enum::enum_name(read_error.ec), " : ", read_error.custom_error_message);
				delete data;
				return nullptr;
			}

			return data;
		}

		std::string GetDefaultPath() override
		{
			return (std::filesystem::path("Stations") / "DEFAULT.json").string();
		}

	private:
		StationDatabase() : AssetDatabase("StationDatabase") {}

	};
}