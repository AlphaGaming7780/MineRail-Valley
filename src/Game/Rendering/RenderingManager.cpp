#include <Game/Rendering/RenderingManager.hpp>
#include <Game/World.h>
#include <magic_enum/magic_enum.hpp>
#include <Game/Rendering/GameWindow.hpp>

namespace Game
{
    RenderingManager& RenderingManager::Instance()
    {
        static RenderingManager instance;
        return instance;
    }

    void RenderingManager::Render(sf::RenderWindow& window)
    {
        window.setView(m_Camera.GetView());

        for (auto& [layer, vec] : m_RenderList)
            vec.clear();

        const auto& gameObjects = World::Instance().GetAllGameObject();

        for (GameObject* obj : gameObjects)
        {
            if (!obj->m_Sprite)
                continue;

            m_RenderList[obj->m_RenderLayer].push_back(obj);
        }

        for (auto& [layer, vec] : m_RenderList)
        {
            std::sort(vec.begin(), vec.end(),
                [](GameObject* a, GameObject* b)
                {
                    return a->zIndex < b->zIndex;
                });
        }

        for (int layer = 0; layer < (int)RenderLayer::COUNT; ++layer)
        {
            auto& vec = m_RenderList[(RenderLayer)layer];
            for (GameObject* obj : vec)
            {
                window.draw(*obj->m_Sprite);
            }
        }

        for (auto& [layer, vec] : m_RenderList)
            vec.clear();
    }
    void RenderingManager::NewFrame()
    {
        GameWindow& gw = GameWindow::Instance();
        gw.Clear();
        gw.Get().setView(m_Camera.GetView());
    }
}