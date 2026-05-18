#pragma once

#include <PallasEngine/PallasEngine.hpp>

// Rework import stuff for the "game"
#include <Game/UI/LoadingScreen.hpp>
#include <Game/InputsActionSets.hpp>
#include <Game/GameStates.h>

namespace Game
{
    class GameManager : public pallas::IGame, public pallas::CoroutineOwner
    {
    public:

        pallas::Event<GameMode, Purpose> OnGameLoadingComplete;
        pallas::Event<GameMode, Purpose> OnGameLoadingStart;

        GameManager();
        ~GameManager();

        static GameManager& Instance();

        void onStart() override;
        void onUpdate() override;
        void onShutdown() override;

        GameState GetCurrentState() const { return m_CurrentState; }
		GameMode GetCurrentMode() const { return m_CurrentMode; }

    private:
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
