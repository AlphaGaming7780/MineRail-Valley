#include <Game/GameManager.hpp>
#include <magic_enum/magic_enum.hpp>
#include <Game/Systems.hpp>

namespace Game
{
    GameManager* GameManager::instance = nullptr;

    GameManager::GameManager()
    {
        if (instance != nullptr)
        {
            m_Logger.Error("GameManager instance already exists!");
            throw std::runtime_error("GameManager instance already exists!");
        }

        instance = this;
    }

    GameManager::~GameManager()
    {
        if (instance == this)
            instance = nullptr;
    }

    GameManager& GameManager::Instance()
    {
        return *instance;
    }

    void GameManager::onStart()
    {
        m_Logger.Info("onStart");

        pallas::GameWindow::Instance().SetTitle("MyGame");

        pallas::InputManager::Instance().Register<GameInputs>();
        pallas::InputManager::Instance().EnableBlock<GameActions>();

		pallas::World::Instance().GetOrCreateSystem<CameraSystem>();

        testTexture = pallas::TextureAsset::GetDefault()->LoadObject();

        StartCoroutine(onStartCoroutine());
    }

    void GameManager::onUpdate()
    {

    }


    void GameManager::onShutdown()
    {
		m_Logger.Info("onShutdown");
        m_CurrentState = GameState::Terminated;
        testTexture = nullptr;
        pallas::TextureAsset::GetDefault()->Unload();
    }

    pallas::Coroutine GameManager::onStartCoroutine()
    {
        UILoadingScreen& loadingScreen = pallas::UIManager::Instance().SetRoot<UILoadingScreen>();

        //for (int i = 1; i <= 100; i++)
        //{
        //    loadingScreen.SetValue(i / 100.f);
        //    co_await pallas::Waiters::WaitForSeconds{ 0.1 };
        //}

        m_Logger.Info("GameLoading done.");

		// Show MainMenu here, for now just reset the UI
        pallas::UIManager::Instance().ResetRoot();

		co_await pallas::Waiters::Join{ MainMenu() };

		co_await Load(GameMode::InGame, Purpose::NewGame);

        m_Logger.Info("onStartCoroutine done");

        auto waterTexture = pallas::AssetDatabase::Instance().GetAsset<pallas::TextureAsset>("Textures\\water.png");

        for (int i = 0; i < 25; i++) {
            auto& world = pallas::World::Instance();
            entt::entity e = world.m_Registry.create();

            float offsetX = 100;
            float offsetY = 100;

            pallas::Transform t;

            float tile = 64.f;
            //float diag = tile / std::sqrt(2.f);

            //float gx = i % 5;
            //float gy = i / 5;

            //float x = (gx - gy) * diag;
            //float y = (gx + gy) * diag;

            float x = tile * (i % 5);
            float y = tile * (i / 5);

            t.m_Position = { offsetX + x, offsetY + y };
            t.m_Origin = { tile / 2.f, tile / 2.f };
            //t.m_Rotation = sf::degrees(45);
            //t.m_Scale = { 2.f, 2.f };
            t.m_Size = { tile, tile };

            world.m_Registry.emplace<pallas::Transform>(e, t);

            pallas::Renderer r;
            r.m_Color = sf::Color(63, 118, 228);
            r.m_Texture = waterTexture->LoadObject();
            r.m_zIndex = 1;
            world.m_Registry.emplace<pallas::Renderer>(e, r);

            pallas:: Animated a;
            a.m_CurrentTime = 2 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            a.m_Duration = 2.f;
            a.m_FramesY = 32;
            world.m_Registry.emplace<pallas::Animated>(e, a);
        }

        pallas::UIManager::Instance().ResetRoot();

    }

    pallas::Coroutine GameManager::MainMenu()
    {
		m_Logger.Info("MainMenu");
        co_await Load(GameMode::MainMenu, Purpose::Cleanup);
		m_Logger.Info("MainMenu loaded");
    }

    pallas::Coroutine GameManager::Load(GameMode gameMode, Purpose purpose)
    {
        m_Logger.InfoO("GameLoading started, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));

        pallas::World::Instance().Pause();
		GameMode oldMode = m_CurrentMode;
		m_CurrentMode = gameMode;
		m_CurrentState = GameState::Loading;

		pallas::UIManager& uiManager = pallas::UIManager::Instance();

        UILoadingScreen& loadingScreen = uiManager.SetRoot<UILoadingScreen>();

        float n = (float)(OnGameLoadingStart.count() + OnGameLoadingComplete.count());
        int i = 0;

		//OnGameLoadingStart(gameMode, purpose);

        for (auto& x : OnGameLoadingStart)
        {
            x(gameMode, purpose);
            i++;
            loadingScreen.SetValue(i / n);
            co_await pallas::Waiters::NextFrame{};
        }

        // Maybe do more stuff here ?
        // Like serialization (loading save game or other stuff);


        //OnGameLoadingComplete(gameMode, purpose);

        for (auto& x : OnGameLoadingComplete)
        {
            x(gameMode, purpose);
            i++;
            loadingScreen.SetValue(i / n);
            co_await pallas::Waiters::NextFrame{};
        }

        pallas::World::Instance().UnPause();
		m_CurrentState = GameState::WorldReady;

		m_Logger.InfoO("GameLoading done, GameMode: ", magic_enum::enum_name(gameMode), " | Purpose: ", magic_enum::enum_name(purpose));
    }
}