#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
	struct TileData : public ObjectData
	{
		bool m_CanBuild = false;
	};
}

namespace glz
{
    template<>
    struct meta<Game::TileData>
    {
        using T = Game::TileData;

        static constexpr auto value = glz::merge(
            glz::meta<Game::ObjectData>::value,
            glz::object(&T::m_CanBuild) // Auto-reflects the name
        );
    };
}