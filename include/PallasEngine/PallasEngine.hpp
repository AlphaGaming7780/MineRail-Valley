#pragma once

#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>
#include <PallasEngine/ECS.hpp>
#include <PallasEngine/Logging.hpp>
#include <PallasEngine/UI.hpp>
#include <PallasEngine/Rendering.hpp>
#include <PallasEngine/Utils.hpp>

#include <PallasEngine/Events/EventManager.hpp>
#include <PallasEngine/Events/InputManager.hpp>
#include <PallasEngine/Events/InputActionSet.hpp>

#include <PallasEngine/IGame.hpp>

namespace pallas
{
	class PallasEngine
	{
	public:
		template<typename TGame>
		static int Start();

	private:

		static Generator<bool> Init();
		static void AddEngineSystems(World& world);
		static void ShowBootstrapUI(UIManager& ui, GameWindow& gw);
		static void MainLoop(IGame& game);
		static void OnClose(const sf::Event::Closed& event);
	};

	template<typename TGame>
	inline int PallasEngine::Start()
	{
		// Init du LogManager
		LogManager::instance();
		Logger logger = Logger("PallasEngine");
		logger.InfoO("PallasEngine starting");

		// Init du singleton ici comme ca c'est l'avant dernier a ce faire détruire.
		AssetDatabase::Instance();

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
		ev.on<sf::Event::Closed>(OnClose);

		// Init de l'InputManager
		auto& inputManager = InputManager::Instance();

		// Init UIManager + creation du bootstrap UI
		auto& ui = UIManager::Instance();
		ShowBootstrapUI(ui, gw);

		// Le menu bootstrap sera supprimer quand le dev affichera sont menu ou chargement.

		// Init du coroutine scheduler
		CoroutineScheduler::Instance();

		RenderingManager& renderManager = RenderingManager::Instance();

		// Init du World ECS, Peut être déplacer après le Init ou alors faire dans le init (AddEngineSystems).
		World& world = World::Instance();
		AddEngineSystems(world);

		// Fonction "async" pour Init/Load les gros truc (genre database)
		auto init = Init();

		// Init loop.
		while (gw.IsOpen() && init.next())
		{
			ev.process();
			//inputManager.Update(); // faut le mettre pour reset le flanc de touche meme | y a pas de binding pour le moment dcp non pas besoin

			CoroutineScheduler::Instance().Update();

			ui.Draw(gw.Get());
			gw.Display();

#ifdef  DEBUG
			PerformanceLogger::Get().SaveToFile();
#endif //  DEBUG

			// For delta time calculation
			Time::Update();
		}

		Time::Reset();
		{
			TGame game;
			MainLoop(game);
			world.Shutdown(); // Shutdown du monde avant TGame pour que les systèmes soient encore là pour faire leur cleanup si besoin.
		}

		return 0;
	}
}