#include <Game/GameManager.hpp>
#include <magic_enum/magic_enum.hpp>
#include <Game/Systems.hpp>
#include <Game/Rendering/RenderingManager.hpp>
#include <Game/AssetDatabase/MapDatabase.h>
#include <Game/UI/Menus/InGameUI.h>

extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // NVIDIA
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // AMD
}

namespace Game
{
	int GameManager::Start()
	{
		// Init du LogManager
		pallas::LogManager::instance();
		m_Logger.InfoO("GameManager starting");

		// Init de la window SFML
		auto& gw = GameWindow::Instance();

		if (!gw.Create("MineRail Valley starting..."))
		{
			m_Logger.ErrorO("Failed to create the SFML window.");
			return 1;
		}
		m_Logger.InfoO("SFML window created.");

		// Init de l'EventManager
		auto& ev = EventManager::Instance().initialize(gw.Get());

		RenderingManager& renderManager = RenderingManager::Instance();

		Time::Reset();
		{
			MainLoop();
		}

		World::Instance().Shutdown();

		UIManager::Instance().ResetRoot();

		TextureDatabase::Instance().UnloadAssets();
		SoundDatabase::Instance().UnloadAssets();
		MusicDatabase::Instance().UnloadAssets();
		FontDatabase::Instance().UnloadAssets();
		TileDatabase::Instance().UnloadAssets();
		MapDatabase::Instance().UnloadAssets();

		return 0;
	}

    void GameManager::OnStart()
    {
        m_Logger.Info("OnStart");

		GameWindow& win = GameWindow::Instance();
		win.SetTitle("MineRail Valley");
		win.ToggleFullscreen();

		World::Instance().GetOrCreateSystem<CameraSystem>();
		World::Instance().GetOrCreateSystem<TrackToolSystem>();
		World::Instance().GetOrCreateSystem<TrainSystem>();
     
		Load(GameMode::InGame, Purpose::NewGame, MapDatabase::Instance().Load("Maps\\VolcanicWastes.json"));
		UIManager::Instance().SetRoot<InGameUI>();
    }

    void GameManager::OnUpdate()
    {

    }

    void GameManager::OnShutdown()
    {
		m_Logger.Info("OnShutdown");
    }

    void GameManager::MainLoop()
	{
		GameWindow& gameWindow = GameWindow::Instance();
		EventManager& eventManager = EventManager::Instance();
		InputManager& inputManager = InputManager::Instance();
		AudioManager& audioManager = AudioManager::Instance();
		RenderingManager& renderManager = RenderingManager::Instance();
		UIManager& uiManager = UIManager::Instance();
		World& world = World::Instance();

		OnStart();

		while (gameWindow.IsOpen())
		{
			renderManager.NewFrame();
			inputManager.ResetEdges();
			eventManager.process();
			inputManager.Update();
			audioManager.Update();

			OnUpdate();

			world.Update();

			uiManager.Update(gameWindow.Get());

			// Rendering part
			renderManager.Render(gameWindow.Get());
			uiManager.Draw(gameWindow.Get());
			gameWindow.Display();
			//

#ifdef  DEBUG
			pallas::PerformanceLogger::Get().SaveToFile();
#endif //  DEBUG

			Time::Update();
		}

		OnShutdown();

	}

    void GameManager::Load(GameMode gameMode, Purpose purpose, MapData* mapData)
    {
        m_Logger.InfoO("GameLoading started, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));

        World::Instance().Pause();
		GameMode oldMode = m_CurrentMode;
		m_CurrentMode = gameMode;
		m_CurrentState = GameState::Loading;

		EventManager& eventManager = EventManager::Instance();
		UIManager& uiManager = UIManager::Instance();
		GameWindow& gameWindow = GameWindow::Instance();
        UILoadingScreen& loadingScreen = uiManager.SetRoot<UILoadingScreen>();

		auto loadingStartObservers = eventManager.GetObservers<LoadingStart>();
		auto loadingCompleteObservers = eventManager.GetObservers<LoadingComplete>();

		float total = loadingStartObservers.size() + loadingCompleteObservers.size();
		int current = 0;

		LoadingStart ls(purpose, gameMode, mapData);
		LoadingComplete lc(purpose, gameMode, mapData);

		for (IEventObserver<LoadingStart>* obs : loadingStartObservers)
		{
			obs->OnEvent(ls);
			loadingScreen.SetValue(++current / total);
			eventManager.process();
			uiManager.Update(gameWindow.Get());
			uiManager.Draw(gameWindow.Get());
			gameWindow.Display();
		}

		for (IEventObserver<LoadingComplete>* obs : loadingCompleteObservers)
		{
			obs->OnEvent(lc);
			loadingScreen.SetValue(++current / total);
			eventManager.process();
			uiManager.Update(gameWindow.Get());
			uiManager.Draw(gameWindow.Get());
			gameWindow.Display();
		}

        World::Instance().UnPause();
		m_CurrentState = GameState::WorldReady;

		m_Logger.InfoO("GameLoading done, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));
    }
}
