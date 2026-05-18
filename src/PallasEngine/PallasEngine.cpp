 //used to force tell the OS we want to run on the main GPU.
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // NVIDIA
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // AMD
}

#include <PallasEngine/PallasEngine.hpp>
#include <PallasEngine/UI/UIGroups/UIBootstrap.hpp>

namespace pallas
{
	Generator<bool> PallasEngine::Init()
	{
		// Create an async load methode later, and wait it.
		AssetDatabase::Instance().Load();

		co_return true;
	}

	void PallasEngine::AddEngineSystems(World& world)
	{
		world.GetOrCreateSystem<SpriteRenderSystem>();
		world.GetOrCreateSystem<CameraSyncInSystem>();
		world.GetOrCreateSystem<CameraSyncOutSystem>();

		return;
	}

	void PallasEngine::ShowBootstrapUI(UIManager& ui, GameWindow& gw)
	{
		ui.SetRoot<UIBootstrap>();
		ui.Draw(gw.Get());
		gw.Display();
	}

	void PallasEngine::MainLoop(IGame& game)
	{
		auto& gameWindow	= GameWindow::Instance();
		auto& eventManager	= EventManager::Instance();
		auto& inputManager	= InputManager::Instance();
		auto& renderManager = RenderingManager::Instance();
		auto& uiManager		= UIManager::Instance();
		auto& ecsWorld		= World::Instance();

		game.onStart();

		//Main Loop
		while (gameWindow.IsOpen())
		{
			gameWindow.Clear(sf::Color::Black);
			inputManager.ResetEdges();
			eventManager.process();
			inputManager.Update();

			CoroutineScheduler::Instance().Update();

			game.onUpdate();

			// ECS world Update.
			ecsWorld.Update();

			uiManager.Update(gameWindow.Get());

			// Rendering part
			renderManager.Render(gameWindow.Get());
			uiManager.Draw(gameWindow.Get());
			gameWindow.Display();

#ifdef  DEBUG
			PerformanceLogger::Get().SaveToFile();
#endif //  DEBUG

			Time::Update();
		}

		game.onShutdown();

	}

	void PallasEngine::OnClose(const sf::Event::Closed& event)
	{
		GameWindow::Instance().Close();
	}
}
