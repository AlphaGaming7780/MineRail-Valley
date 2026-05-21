#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
    struct TrainData
    {
        ObjectData object;
    };
}

namespace glz
{
    template<>
    struct meta<Game::TrainData>
    {
        using T = Game::TrainData;

        static constexpr auto value = glz::object(
            "ObjectData", &T::object
        );
    };
}