#pragma once

// Existing game-loop dependencies (kept identical to the previous version)
#include <Game/UI/Menus/LoadingScreen.hpp>
#include <Game/InputsActionSets.hpp>
#include <Game/GameStates.h>
#include <Game/World.h>
#include <Game/Utils.hpp>
#include <Game/Events.h>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/UI/UIManager.hpp>
#include <Game/AudioManager.hpp>
#include <Game/AssetDatabase/AssetType/MapData.h>

// New: full menu/screen stack
#include <Game/UI/Menus/TitleScreen.hpp>
#include <Game/UI/Menus/LevelSelectScreen.hpp>
#include <Game/UI/Menus/SettingsScreen.hpp>
#include <Game/UI/Menus/CreditsScreen.hpp>
#include <Game/UI/Menus/PauseMenu.hpp>
#include <Game/UI/Menus/VictoryScreen.hpp>
#include <Game/UI/Menus/GameOverScreen.hpp>
#include <Game/UI/Widgets/UIMusicPlayer.hpp>

#include <vector>
#include <string>

namespace Game
{
    // Logical screen the player is currently viewing. Drives Show*() routing.
    enum class Screen
    {
        None,
        Title,
        LevelSelect,
        Settings,
        Credits,
        InGame,
        Pause,
        Victory,
        GameOver
    };

    class GameManager
    {
    public:

        static GameManager& Instance() {
            static GameManager inst;
            return inst;
        }

        ~GameManager() = default;

        int Start();
        void OnStart();
        void OnUpdate();
        void OnShutdown();

        GameState GetCurrentState() const { return m_CurrentState; }
        GameMode  GetCurrentMode()  const { return m_CurrentMode; }
        Screen    GetCurrentScreen() const { return m_CurrentScreen; }

        // ── Screen navigation ─────────────────────────────────────────────────
        void ShowTitle();
        void ShowLevelSelect();
        // returnTo decides whether Back goes to Title or Pause.
        void ShowSettings(Screen returnTo);
        void ShowCredits();
        void ShowInGameHUD();
        void ShowPause();
        void ShowVictory(const std::vector<VictoryScreen::StatLine>& stats);
        void ShowGameOver(const std::string& reason = {});

        // ── Level lifecycle ───────────────────────────────────────────────────
        void LoadLevel(const std::string& mapPath);
        void ReloadCurrentLevel();
        void LoadNextLevel();
        void TeardownLevel();

        // ── App lifecycle ─────────────────────────────────────────────────────
        void RequestQuit();

    private:
        GameManager() = default;

        pallas::Logger m_Logger = pallas::Logger("GameManager");

        GameState m_CurrentState  = GameState::Booting;
        GameMode  m_CurrentMode   = GameMode::None;
        Screen    m_CurrentScreen = Screen::None;

        // Settings flow context: SettingsScreen.Back returns to this screen.
        Screen    m_SettingsReturnTo = Screen::Title;

        // Current level for replay / next-level routing.
        std::string m_CurrentMapPath;
        std::vector<std::string> m_LevelOrder;

        // Pointer to the music player owned by the current screen (HUD or Pause).
        // Reset to nullptr in every Show* that doesn't host one; refreshed in
        // WireMusicPlayer for those that do.
        UIMusicPlayer* m_CurrentMusicPlayer = nullptr;

        // Cache of last-pushed values so UpdateMusicPlayerWidget can avoid
        // redundant SetText / SetProgress calls (which trigger ReDraw).
        std::string m_LastTrackName;
        bool        m_LastPlayingState = false;
        int         m_LastProgressBucket = -1;
        int         m_LastSecondsCurrent = -1;
        int         m_LastSecondsTotal   = -1;

        void MainLoop();
        void Load(GameMode gameMode, Purpose purpose, MapData* mapData = nullptr);

        // Bind a freshly-instantiated music player to AudioManager + remember it
        // for the per-frame UpdateMusicPlayerWidget() refresh.
        void WireMusicPlayer(UIMusicPlayer* mp);
        void UpdateMusicPlayerWidget();
    };
}
