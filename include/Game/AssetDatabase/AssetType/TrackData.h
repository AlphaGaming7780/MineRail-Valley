#pragma once
#include <string>
#include <glaze/glaze.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>

namespace Game
{
	struct TrackData : public ObjectData
	{
        ObjectData object;
        std::string m_StraightTrackTexture;
        std::string m_BendTrackTexture;
	};
}

namespace glz
{
    template<>
    struct meta<Game::TrackData>
    {
        using T = Game::TrackData;

        static constexpr auto value = glz::object(
            "ObjectData", &T::object,
            "StraightTrackTexture", &T::m_StraightTrackTexture,
            "BendTrackTexture", &T::m_BendTrackTexture
        );
    };
}
