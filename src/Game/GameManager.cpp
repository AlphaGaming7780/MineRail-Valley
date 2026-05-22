#include <Game/GameManager.hpp>
#include <magic_enum/magic_enum.hpp>
#include <Game/Systems.hpp>
#include <Game/Rendering/RenderingManager.hpp>
#include <Game/AssetDatabase/MapDatabase.h>
#include <Game/UI/Menus/InGameUI.h>
#include <Game/Settings/SettingsManager.hpp>

#include <algorithm>

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

		// ── Settings: load from disk + apply to subsystems ─────────────────
		auto& settings = SettingsManager::Instance();
		settings.Load();                  // failure → keeps defaults
		settings.Apply();                 // pushes volumes to AudioManager
		if (settings.Get().Fullscreen)    // fullscreen toggled here, once.
			win.ToggleFullscreen();

		// ── Build the world ECS systems (idle until a level loads) ─────────
		World::Instance().GetOrCreateSystem<CameraSystem>();
		World::Instance().GetOrCreateSystem<TrackToolSystem>();
		World::Instance().GetOrCreateSystem<TrainSystem>();
<<<<<<< Updated upstream
     
		Load(GameMode::InGame, Purpose::NewGame, MapDatabase::Instance().Load("Maps\\ForestValley.json"));
		UIManager::Instance().SetRoot<InGameUI>();
=======

		// ── Level navigation order ────────────────────────────────────────
		// Used by LoadNextLevel() after Victory. Order = difficulty-ish.
		m_LevelOrder = {
			"Maps\\Plain.json",
			"Maps\\ForestValley.json",
			"Maps\\FrozenHighlands.json",
			"Maps\\DesertCanyon.json",
			"Maps\\VolcanicWastes.json",
			"Maps\\NeonCity.json",
		};

		// ── Start at the title screen ─────────────────────────────────────
		ShowTitle();
>>>>>>> Stashed changes
    }

    void GameManager::OnUpdate()
    {
		UpdateMusicPlayerWidget();
    }

    void GameManager::OnShutdown()
    {
		m_Logger.Info("OnShutdown");
		// Best-effort save (in case the player exited via the window X).
		SettingsManager::Instance().Save();
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

		// Stale music player pointer is about to become dangling — the loading
		// screen replaces the root, which deletes the previous tree.
		m_CurrentMusicPlayer = nullptr;

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

		m_CurrentState = GameState::WorldReady;

		m_Logger.InfoO("GameLoading done, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));
    }

	// ════════════════════════════════════════════════════════════════════════
	//   Screen navigation
	// ════════════════════════════════════════════════════════════════════════

	void GameManager::ShowTitle()
	{
		// Only re-fire the loading sequence (which swaps to the 0.Menus playlist)
		// when we actually come from a non-menu context. Going Title→Settings→
		// Title shouldn't restart menu music.
		if (m_CurrentMode != GameMode::MainMenu)
		{
			Load(GameMode::MainMenu, Purpose::Cleanup, nullptr);
		}
		m_CurrentScreen = Screen::Title;
		m_CurrentMusicPlayer = nullptr;

		auto& t = UIManager::Instance().SetRoot<TitleScreen>();
		t.SetOnPlay    ([this]{ ShowLevelSelect(); });
		t.SetOnSettings([this]{ ShowSettings(Screen::Title); });
		t.SetOnCredits ([this]{ ShowCredits(); });
		t.SetOnQuit    ([this]{ RequestQuit(); });
	}

	void GameManager::ShowLevelSelect()
	{
		m_CurrentScreen = Screen::LevelSelect;
		m_CurrentMusicPlayer = nullptr;

		auto& ls = UIManager::Instance().SetRoot<LevelSelectScreen>();
		ls.SetOnPlayLevel([this](const std::string& mp){ LoadLevel(mp); });
		ls.SetOnBack     ([this]{ ShowTitle(); });
	}

	void GameManager::ShowSettings(Screen returnTo)
	{
		m_SettingsReturnTo = returnTo;
		m_CurrentScreen = Screen::Settings;
		m_CurrentMusicPlayer = nullptr;

		auto& s = UIManager::Instance().SetRoot<SettingsScreen>();
		s.SetCurrent(SettingsManager::Instance().Get());
		s.SetOnApply([this](const GameSettings& ns){
			SettingsManager::Instance().Set(ns);
			SettingsManager::Instance().Apply();
			SettingsManager::Instance().Save();
			// Note: fullscreen toggle is not auto-applied here. Apply takes
			// effect on next ShowTitle() boot. (Live toggling needs a
			// dedicated GameWindow API that handles ToggleFullscreen correctly
			// even when already fullscreen.)
		});
		s.SetOnBack([this]{
			if (m_SettingsReturnTo == Screen::Pause)
				ShowPause();
			else
				ShowTitle();
		});
	}

	void GameManager::ShowCredits()
	{
		m_CurrentScreen = Screen::Credits;
		m_CurrentMusicPlayer = nullptr;

		auto& c = UIManager::Instance().SetRoot<CreditsScreen>();
		c.SetOnBack([this]{ ShowTitle(); });
	}

	void GameManager::ShowInGameHUD()
	{
		m_CurrentScreen = Screen::InGame;
		World::Instance().Pause(false);

		auto& hud = UIManager::Instance().SetRoot<InGameUI>();
		WireMusicPlayer(hud.GetMusicPlayer());
	}

	void GameManager::ShowPause()
	{
		m_CurrentScreen = Screen::Pause;
		World::Instance().Pause(true);

		auto& p = UIManager::Instance().SetRoot<PauseMenu>();
		p.SetOnResume   ([this]{ ShowInGameHUD(); });
		p.SetOnSettings ([this]{ ShowSettings(Screen::Pause); });
		p.SetOnMainMenu ([this]{ TeardownLevel(); ShowTitle(); });
		p.SetOnQuit     ([this]{ RequestQuit(); });
		WireMusicPlayer(p.GetMusicPlayer());
	}

	void GameManager::ShowVictory(const std::vector<VictoryScreen::StatLine>& stats)
	{
		m_CurrentScreen = Screen::Victory;
		m_CurrentMusicPlayer = nullptr;

		auto& v = UIManager::Instance().SetRoot<VictoryScreen>();
		v.SetStats(stats);
		v.SetOnReplay   ([this]{ ReloadCurrentLevel(); });
		v.SetOnNextLevel([this]{ LoadNextLevel(); });
		v.SetOnMainMenu ([this]{ TeardownLevel(); ShowTitle(); });
	}

	void GameManager::ShowGameOver(const std::string& reason)
	{
		m_CurrentScreen = Screen::GameOver;
		m_CurrentMusicPlayer = nullptr;

		auto& over = UIManager::Instance().SetRoot<GameOverScreen>();
		over.SetReason(reason);
		over.SetOnRetry   ([this]{ ReloadCurrentLevel(); });
		over.SetOnMainMenu([this]{ TeardownLevel(); ShowTitle(); });
	}

	// ════════════════════════════════════════════════════════════════════════
	//   Level lifecycle
	// ════════════════════════════════════════════════════════════════════════

	void GameManager::LoadLevel(const std::string& mapPath)
	{
		m_CurrentMapPath = mapPath;
		MapData* md = MapDatabase::Instance().Load(mapPath);
		if (md == nullptr)
		{
			m_Logger.ErrorO("LoadLevel: failed to load map: ", mapPath);
			ShowTitle();
			return;
		}
		Load(GameMode::InGame, Purpose::NewGame, md);
		ShowInGameHUD();
	}

	void GameManager::ReloadCurrentLevel()
	{
		if (m_CurrentMapPath.empty()) { ShowTitle(); return; }
		LoadLevel(m_CurrentMapPath);
	}

	void GameManager::LoadNextLevel()
	{
		if (m_LevelOrder.empty()) { ShowTitle(); return; }
		auto it = std::find(m_LevelOrder.begin(), m_LevelOrder.end(), m_CurrentMapPath);
		if (it == m_LevelOrder.end()) {
			// Unknown map — restart from the first.
			LoadLevel(m_LevelOrder.front());
			return;
		}
		++it;
		if (it == m_LevelOrder.end()) {
			// Beat the last level — go back to title (or trophy screen).
			ShowTitle();
			return;
		}
		LoadLevel(*it);
	}

	void GameManager::TeardownLevel()
	{
		// Unpause in case we're coming from Pause; the next level load will
		// re-pause via LoadingStart→LoadingComplete observers anyway.
		World::Instance().Pause(false);
		m_CurrentMapPath.clear();
	}

	// ════════════════════════════════════════════════════════════════════════
	//   App lifecycle
	// ════════════════════════════════════════════════════════════════════════

	void GameManager::RequestQuit()
	{
		// Persist settings on the way out so volume changes during play stick.
		SettingsManager::Instance().Save();
		GameWindow::Instance().Close();
	}

	// ════════════════════════════════════════════════════════════════════════
	//   Music player wiring
	// ════════════════════════════════════════════════════════════════════════

	void GameManager::WireMusicPlayer(UIMusicPlayer* mp)
	{
		m_CurrentMusicPlayer = mp;
		// Reset the cached state so the next UpdateMusicPlayerWidget pushes
		// a full refresh into the new widget.
		m_LastTrackName.clear();
		m_LastPlayingState = false;
		m_LastProgressBucket = -1;
		m_LastSecondsCurrent = -1;
		m_LastSecondsTotal = -1;
		if (mp == nullptr) return;

		auto& am = AudioManager::Instance();
		mp->SetOnPlayPause([&am]{ am.TogglePlaylistPause(); });
		mp->SetOnNext     ([&am]{ am.AdvancePlaylist();      });
		mp->SetOnPrev     ([&am]{ am.RewindPlaylist();       });
		mp->SetOnStop     ([&am]{ am.StopPlaylist();         });
	}

	namespace
	{
		// Format an mm:ss string for the music player time label.
		std::string FormatMMSS(int seconds)
		{
			if (seconds < 0) seconds = 0;
			int m = seconds / 60, s = seconds % 60;
			std::string r = std::to_string(m) + ":";
			if (s < 10) r += "0";
			r += std::to_string(s);
			return r;
		}
	}

	void GameManager::UpdateMusicPlayerWidget()
	{
		if (m_CurrentMusicPlayer == nullptr) return;

		auto& am = AudioManager::Instance();

		// Title
		const std::string name = am.GetCurrentTrackName();
		if (name != m_LastTrackName)
		{
			m_CurrentMusicPlayer->SetTrackTitle(name);
			m_LastTrackName = name;
		}

		// Play/Pause state
		const bool playing = am.IsPlaylistPlaying();
		if (playing != m_LastPlayingState)
		{
			m_CurrentMusicPlayer->SetPlaying(playing);
			m_LastPlayingState = playing;
		}

		// Progress (bucket to 100 to avoid spamming ReDraw on every frame)
		const float p01 = am.GetCurrentTrackPosition01();
		const int bucket = static_cast<int>(p01 * 100.f);
		if (bucket != m_LastProgressBucket)
		{
			m_CurrentMusicPlayer->SetProgress(p01);
			m_LastProgressBucket = bucket;
		}

		// Time label
		const int curS = static_cast<int>(am.GetCurrentTrackTime().asSeconds());
		const int totS = static_cast<int>(am.GetCurrentTrackDuration().asSeconds());
		if (curS != m_LastSecondsCurrent || totS != m_LastSecondsTotal)
		{
			m_CurrentMusicPlayer->SetTimeLabels(FormatMMSS(curS), FormatMMSS(totS));
			m_LastSecondsCurrent = curS;
			m_LastSecondsTotal = totS;
		}
	}
}
