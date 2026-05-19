#pragma once

// Rework import stuff for the "game"
#include <Game/UI/Menus/LoadingScreen.hpp>
#include <Game/InputsActionSets.hpp>
#include <Game/GameStates.h>
#include <Game/World.h>
#include <Game/Utils.hpp>
#include <Game/Events.h>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/UI/UIManager.hpp>
#include <Game/AudioManager.hpp>

namespace Game
{
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
		GameMode GetCurrentMode() const { return m_CurrentMode; }

    private:
        GameManager();

        pallas::Logger m_Logger = pallas::Logger("GameManager");

        GameState m_CurrentState = GameState::Booting;
		GameMode m_CurrentMode = GameMode::None;

        void MainLoop();

        void Load(GameMode gameMode, Purpose purpose);
    };
}
