#include <Game/Settings/SettingsManager.hpp>
#include <Game/AudioManager.hpp>
#include <glaze/glaze.hpp>
#include <filesystem>

namespace Game
{
    namespace
    {
        std::string ResolveAssetsPath(const std::string& rel)
        {
            namespace fs = std::filesystem;
            return (fs::current_path() / "assets" / rel).string();
        }
    }

    bool SettingsManager::Load(const std::string& relPath)
    {
        const std::string full = ResolveAssetsPath(relPath);
        auto err = glz::read_file_json(m_Settings, full, std::string{});
        if (err)
        {
            m_Logger.WarnO("[SettingsManager] Failed to load: ", full,
                           " — using defaults. Reason: ", err.custom_error_message);
            return false;
        }
        m_Logger.InfoO("[SettingsManager] Loaded ", full);
        return true;
    }

    bool SettingsManager::Save(const std::string& relPath) const
    {
        const std::string full = ResolveAssetsPath(relPath);
        // Ensure the parent directory exists (assets/Settings/).
        try { std::filesystem::create_directories(std::filesystem::path(full).parent_path()); }
        catch (...) { /* best-effort */ }

        auto err = glz::write_file_json(m_Settings, full, std::string{});
        if (err)
        {
            m_Logger.ErrorO("[SettingsManager] Failed to save: ", full,
                            " — ", err.custom_error_message);
            return false;
        }
        return true;
    }

    void SettingsManager::Apply()
    {
        auto& am = AudioManager::Instance();
        am.SetMasterVolume(m_Settings.MasterVolume);
        am.SetSoundVolume (m_Settings.SoundVolume);
        am.SetMusicVolume (m_Settings.MusicVolume);
        // Fullscreen / VSync are handled by the caller (see GameManager) because
        // they require interacting with GameWindow at well-defined transition
        // points, not during ad-hoc Apply() calls.
    }
}
