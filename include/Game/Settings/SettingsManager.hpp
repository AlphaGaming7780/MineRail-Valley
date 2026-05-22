#pragma once

#include <Game/Settings/GameSettings.hpp>
#include <PallasEngine/Logging.hpp>
#include <string>

namespace Game
{
    // SettingsManager: singleton owning the live GameSettings instance.
    //
    // Lifecycle:
    //   - GameManager::OnStart() calls Load() then Apply().
    //   - SettingsScreen edits the GameSettings via SetCurrent / ReadInto,
    //     then GameManager calls Set() + Apply() + Save() on Apply.
    //   - On graceful shutdown, GameManager::RequestQuit() calls Save() again
    //     so the last-known volume / fullscreen state survives the session.
    //
    // The default path is "Settings\\settings.json" under the assets root used
    // by the asset databases (resolved via std::filesystem::current_path()/assets).
    class SettingsManager
    {
    public:
        static SettingsManager& Instance()
        {
            static SettingsManager inst;
            return inst;
        }

        bool Load(const std::string& relativePath = "Settings\\settings.json");
        bool Save(const std::string& relativePath = "Settings\\settings.json") const;

        const GameSettings& Get() const { return m_Settings; }
        void Set(const GameSettings& s) { m_Settings = s; }

        // Push the current settings to the live subsystems (AudioManager etc.).
        // Window fullscreen/vsync isn't applied here — call GameWindow APIs
        // separately from your transition code if those toggles change.
        void Apply();

    private:
        SettingsManager() = default;
        GameSettings m_Settings;
        pallas::Logger m_Logger = pallas::Logger("SettingsManager");
    };
}
