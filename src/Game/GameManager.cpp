#include <Game/GameManager.hpp>
#include <magic_enum/magic_enum.hpp>
#include <Game/Systems.hpp>

namespace Game
{
	int GameManager::Start()
	{
		// Init du LogManager
		pallas::LogManager::instance();
		pallas::Logger logger = pallas::Logger("PallasEngine");
		logger.InfoO("PallasEngine starting");

		// Init de la window SFML
		auto& gw = GameWindow::Instance();

		if (!gw.Create("PallasEngine starting..."))
		{
			logger.ErrorO("Failed to create the SFML window.");
			return 1;
		}
		logger.InfoO("SFML window created.");

		// Init de l'EventManager
		auto& ev = EventManager::Instance().initialize(gw.Get());


		World& world = World::Instance();

		Time::Reset();
		{
			MainLoop();
			world.Shutdown(); // Shutdown du monde avant TGame pour que les systèmes soient encore là pour faire leur cleanup si besoin.
		}

		return 0;
	}

    void GameManager::OnStart()
    {
        m_Logger.Info("Start");

        GameWindow::Instance().SetTitle("MineRail Valley");

		World::Instance().GetOrCreateSystem<CameraSystem>();
        
    }

    void GameManager::OnUpdate()
    {

    }


    void GameManager::OnShutdown()
    {
		m_Logger.Info("Shutdown");
    }

    void GameManager::MainLoop()
	{

		World& world = World::Instance();
		EventManager& eventManager = EventManager::Instance();
		GameWindow& gameWindow = GameWindow::Instance();
		InputManager& inputManager = InputManager::Instance();
		UIManager& uiManager = UIManager::Instance();
		AudioManager& audioManager = AudioManager::Instance();

		OnStart();

		while (gameWindow.IsOpen())
		{
			gameWindow.Clear(sf::Color::Black);
			inputManager.ResetEdges();
			eventManager.process();
			inputManager.Update();

			OnUpdate();

			world.Update();

			uiManager.Update(gameWindow.Get());

			// Rendering part
			//renderManager.Render(gameWindow.Get());
			uiManager.Draw(gameWindow.Get());
			gameWindow.Display();

#ifdef  DEBUG
			pallas::PerformanceLogger::Get().SaveToFile();
#endif //  DEBUG

			Time::Update();
		}

		OnShutdown();

	}

    void GameManager::Load(GameMode gameMode, Purpose purpose)
    {
        m_Logger.InfoO("GameLoading started, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));

        return;

  //      pallas::World::Instance().Pause();
		//GameMode oldMode = m_CurrentMode;
		//m_CurrentMode = gameMode;
		//m_CurrentState = GameState::Loading;

		//pallas::UIManager& uiManager = pallas::UIManager::Instance();

  //      UILoadingScreen& loadingScreen = uiManager.SetRoot<UILoadingScreen>();

  //      float n = (float)(OnGameLoadingStart.count() + OnGameLoadingComplete.count());
  //      int i = 0;

  //      for (auto& x : OnGameLoadingStart)
  //      {
  //          x(gameMode, purpose);
  //          i++;
  //          loadingScreen.SetValue(i / n);
  //      }

  //      // Maybe do more stuff here ?
  //      // Like serialization (loading save game or other stuff);


  //      for (auto& x : OnGameLoadingComplete)
  //      {
  //          x(gameMode, purpose);
  //          i++;
  //          loadingScreen.SetValue(i / n);
  //      }

  //      pallas::World::Instance().UnPause();
		//m_CurrentState = GameState::WorldReady;

		//m_Logger.InfoO("GameLoading done, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));
    }
}