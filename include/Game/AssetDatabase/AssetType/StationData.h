#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
    struct StationData
    {
        ObjectData object;
        std::string m_SationColor;
    };
}

namespace glz
{
    template<> struct meta<Game::StationData>
    {
        using T = Game::StationData;
        static constexpr auto value = glz::object(
            "ObjectData", &T::object,
            "SationColor", &T::m_SationColor
        );
    };
}