#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
	struct TrackData : public ObjectData
	{
        std::string m_Type;
	};
}

namespace glz
{
    template<>
    struct meta<Game::TrackData>
    {
        using T = Game::TrackData;

        static constexpr auto value = glz::merge(
            glz::meta<Game::ObjectData>::value,
            glz::object(&T::m_Type) // Auto-reflects the name
        );
    };
}
