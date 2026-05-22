#pragma once
#include <string>

namespace Game
{
    // GameSettings — user-tunable settings persisted on disk.
    //
    // Storage location:
    //   assets/Settings/settings.json
    //
    // Loaded/saved by your GameManager (TODO — see UI_INTEGRATION.md for an
    // example using glaze::read_file_json / write_file_json, identical to
    // MapData loading).
    //
    // Volume fields are 0..100 to mirror the convention used by AudioManager
    // (which divides by 100 internally to produce a 0..1 multiplier).
    struct GameSettings
    {
        // Audio (0..100)
        float MasterVolume = 70.f;
        float SoundVolume  = 85.f;
        float MusicVolume  = 60.f;

        // Video
        bool Fullscreen = false;
        bool VSync      = true;

        // Last menu music position resume (optional, can be ignored for now)
        std::string LastTrack;
        float       LastTrackPosition = 0.f;
    };
}
