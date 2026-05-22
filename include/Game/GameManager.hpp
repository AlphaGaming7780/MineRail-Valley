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
#include <Game/AssetDatabase/AssetType/MapData.h>
#include <functional>

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

        void MainMenu();

        GameState GetCurrentState() const { return m_CurrentState; }
		GameMode GetCurrentMode() const { return m_CurrentMode; }

        void RequestMainMenu();
        void RequestLoadGame(const std::string mapDataPath);
        void RequestLoadGame(MapData* mapData);

        template<typename TUIGroup>
        void RequestLoad(GameMode mode, Purpose purpose, MapData* mapData)
        {
            m_PendingLoad = [this, mapData, mode, purpose]()
                {
                    this->Load(mode, purpose, mapData);
                    UIManager::Instance().RequestNewRoot<TUIGroup>();
                };
        }

        void Exit();

        MapData* GetCurrentMapData();

    private:
        GameManager() = default;

        pallas::Logger m_Logger = pallas::Logger("GameManager");

        GameState m_CurrentState = GameState::Booting;
		GameMode m_CurrentMode = GameMode::None;

        MapData* m_CurrentMap = nullptr;

        std::function<void()> m_PendingLoad = nullptr;

        void MainLoop();

        void Load(GameMode gameMode, Purpose purpose, MapData* mapData = nullptr);
    };
}
