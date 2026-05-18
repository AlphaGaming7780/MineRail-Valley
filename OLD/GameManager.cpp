#include <Game/GameManager.hpp>

#include <Windows.h>  
#include <iostream>            // Pour std::cout
#include <GL/gl.h>             // Pour glGetString, GL_RENDERER

#include <Game/UI/LoadingScreen.hpp>

using namespace pallas;

sf::Vector2f CameraSize = { 1920, 1080 };

namespace Game {

    GameManager::GameManager() {}
    GameManager::~GameManager() {}

    GameManager& GameManager::getInstance()
    {
        static GameManager instance;
        return instance;
    }

    int GameManager::Setup() {
        m_Logger.Info("Starting.");
        //CameraSize = (sf::Vector2f)sf::VideoMode::getDesktopMode().size;
        m_Logger.InfoO("Screen Size (DesktopMode): ", sf::VideoMode::getDesktopMode().size);
        for (auto& mode : sf::VideoMode::getFullscreenModes()) {
            m_Logger.InfoO("Screen Size (FullscreenMode): ", mode.size);
        }

        db.Load();

        m_World.GetOrCreateSystem<pallas::RenderSystem>();

        m_Logger.Warn("Warn");
        m_Logger.Error("Error");
        window.create(sf::VideoMode::getDesktopMode(), "SFML works!", sf::Style::Default, sf::State::Windowed);
        view = sf::View({ { CameraSize.x /2.f, CameraSize.y /2.f }, { CameraSize.x, CameraSize.y } });
        window.setView(view);

        auto& em = pallas::EventManager::Instance();
        em.initialize(window);
        em.on<sf::Event::Closed>([this](const sf::Event::Closed& ev) {window.close(); });
        em.on<sf::Event::Resized>([this](const sf::Event::Resized& ev) 
            {
                UpdateView({ ev.size.x, ev.size.y });
            }
        );

        //UIManager::Instance().UpdateLayout(window);

        if(!window.setActive(true)) return 1;

        const GLubyte* renderer = glGetString(GL_RENDERER);
        m_Logger.InfoF("Running on GPU : %s", renderer);

        auto loadingScreen = UIManager::Instance().SetRoot<UILoadingScreen>();

        return 0;
    }

    int GameManager::Run() 
    {
        auto tex11 = db.GetAsset<TextureAsset>("PallasEngine\\DEFAULT.png");
        auto tex21 = db.GetAsset<TextureAsset>("Textures\\Test.jpg");
        auto tex31 = db.GetAsset<TextureAsset>("Textures\\water.png");
        TextureAsset* tex12 = nullptr;
        if (!db.TryGetAsset<TextureAsset>("PallasEngine\\DEFAULT.png", tex12)) {
            m_Logger.Error("Failed to get the texture");
        }

        sf::Texture* Textures[] = { tex11->LoadAsset(), tex21->LoadAsset()};

        // Création d'une liste de sprites
        std::vector<sf::Sprite> sprites;
        const int spriteCount = 10;
        for (int x = 0; x < 2; x++) {
            sf::Texture* texture = Textures[x];
            for (int i = 0; i < spriteCount; i++) {
                entt::entity e = m_World.m_Registry.create();
                Transform t;
                float x = static_cast<float>(rand() % static_cast<int>(CameraSize.x));
                float y = static_cast<float>(rand() % static_cast<int>(CameraSize.y));

                t.m_Position = { x, y };
                t.m_Rotation = sf::degrees(rand() % 360);
                t.m_Scale = { 0.1f + (rand() % 100) / 250.f, 0.1f + (rand() % 100) / 250.f };

                m_World.m_Registry.emplace<Transform>(e, t);

                Renderer r;
                r.m_Texture = texture;
                m_World.m_Registry.emplace<Renderer>(e, r);
            }
        }

        for (int i = 0; i < 25; i++) {
            entt::entity e = m_World.m_Registry.create();

            float offsetX = 500;
            float offsetY = 500;

            Transform t;

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
            t.m_Scale = { 2.f, 2.f };

            m_World.m_Registry.emplace<Transform>(e, t);

            Renderer r;
            r.m_Color = sf::Color(63, 118, 228);
            r.m_Texture = tex31->LoadAsset();
            r.m_zIndex = 1;
            m_World.m_Registry.emplace<Renderer>(e, r);

            Animated a;
            a.m_CurrentTime = 2 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            a.m_Duration = 2.f;
            a.m_FramesY = 32;
            m_World.m_Registry.emplace<Animated>(e, a);
        }

        sf::Vector2u pendingResize;
        bool hasResize = false;


        while (window.isOpen())
        {
            pallas::EventManager::Instance().process();

            pallas::CoroutineScheduler::Instance().Update();

            window.clear();
            window.setView(view);
            m_World.Update();
            
            UIManager::Instance().Update(window);

            UIManager::Instance().Draw(window);

#ifdef  DEBUG
            auto perf = PerformanceLogger::Scoped("Window.Display()");
#endif //  DEBUG
            window.display();
#ifdef  DEBUG
            perf.End();
#endif //  DEBUG

#ifdef  DEBUG
            PerformanceLogger::Get().SaveToFile();
#endif //  DEBUG

            Time::Update();
        }

        return 0;
    }

    void GameManager::UpdateView(sf::Vector2u size)
    {
        float windowWidth = static_cast<float>(size.x);
        float windowHeight = static_cast<float>(size.y);

        float windowRatio = windowWidth / windowHeight;
        float targetRatio = (float)CameraSize.x / CameraSize.y;

        view.setCenter((sf::Vector2f)CameraSize / 2.f);

        // Si la fenêtre est plus large → élargir horizontalement
        if (windowRatio > targetRatio)
        {
            float newWidth = CameraSize.y * windowRatio;
            view.setSize({ newWidth, static_cast<float>(CameraSize.y) });
        }
        else
        {
            // Si la fenêtre est plus haute → élargir verticalement
            float newHeight = CameraSize.x / windowRatio;
            view.setSize({ static_cast<float>(CameraSize.x), newHeight });
        }

        // Toujours remplir toute la fenêtre
        view.setViewport({ {0.f, 0.f,}, { 1.f, 1.f } });
    }

    // Fixed ratio - Not used anymore
    //void GameManager::UpdateView(sf::Vector2u size)
    //{
    //    float windowWidth = static_cast<float>(size.x);
    //    float windowHeight = static_cast<float>(size.y);
    //    float aspectRatio = windowWidth / windowHeight;

    //    sf::View view;
    //    view.setSize({ (float)CameraSize.x, (float)CameraSize.y });
    //    view.setCenter({ CameraSize.x / 2.f, CameraSize.y / 2.f });

    //    float targetRatio = (float)CameraSize.x / CameraSize.y;
    //    sf::FloatRect viewport;

    //    if (aspectRatio > targetRatio)
    //    {
    //        float width = targetRatio / aspectRatio;
    //        viewport = { { (1.f - width) / 2.f, 0.f},{ width, 1.f } };
    //    }
    //    else
    //    {
    //        float height = aspectRatio / targetRatio;
    //        viewport = { {0.f, (1.f - height) / 2.f}, {1.f, height } };
    //    }

    //    view.setViewport(viewport);
    //    window.setView(view);
    //}


}