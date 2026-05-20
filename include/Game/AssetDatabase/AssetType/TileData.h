#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
    struct TileData
    {
        ObjectData object;
        bool m_CanBuild = false;
    };
}

namespace glz
{
    template<>
    struct meta<Game::TileData>
    {
        using T = Game::TileData;

        static constexpr auto value = glz::object(
            "ObjectData", &T::object,
            "m_CanBuild", &T::m_CanBuild
        );
    };
}