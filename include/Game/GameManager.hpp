#pragma once

#include <PallasEngine/PallasEngine.hpp>

// Rework import stuff for the "game"
#include <Game/UI/LoadingScreen.hpp>
#include <Game/InputsActionSets.hpp>
#include <Game/GameStates.h>

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

        void Start();
        void Update();
        void Shutdown();

        GameState GetCurrentState() const { return m_CurrentState; }
		GameMode GetCurrentMode() const { return m_CurrentMode; }

    private:
        GameManager();
        static GameManager* instance;

        sf::Texture* testTexture = nullptr;

        pallas::Logger m_Logger = pallas::Logger("GameManager");

        GameState m_CurrentState = GameState::Booting;
		GameMode m_CurrentMode = GameMode::None;

        pallas::Coroutine onStartCoroutine();
        pallas::Coroutine MainMenu();

        pallas::Coroutine Load(GameMode gameMode, Purpose purpose);
    };
}
