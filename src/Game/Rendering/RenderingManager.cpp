#include <Game/Rendering/RenderingManager.hpp>
#include <Game/World.h>
#include <magic_enum/magic_enum.hpp>

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
            m_Logger.InfoO("Rendering ", magic_enum::enum_name((RenderLayer)layer), " with ", vec.size(), " objects.");
            for (GameObject* obj : vec)
            {
                window.draw(*obj->m_Sprite);
            }
        }

        for (auto& [layer, vec] : m_RenderList)
            vec.clear();
    }
}