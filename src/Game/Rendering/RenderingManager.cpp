#include <Game/Rendering/RenderingManager.hpp>
#include <Game/World.h>

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
            for (GameObject* obj : m_RenderList[(RenderLayer)layer])
            {
                window.draw(*obj->m_Sprite);
            }
        }

        for (auto& [layer, vec] : m_RenderList)
            vec.clear();
    }

    sf::Vector2f RenderingManager::GetActualScale(const RenderCommand& cmd)
    {
        if (!cmd.texture)
            return { 0.f, 0.f };

        // Taille brute de la texture ou du frame animé
        sf::Vector2f baseSize;

        if (cmd.animated)
        {
            baseSize = sf::Vector2f(
                static_cast<float>(cmd.frameWidth),
                static_cast<float>(cmd.frameHeight)
            );
        }
        else
        {
            auto texSize = cmd.texture->getSize();
            baseSize = sf::Vector2f(
                static_cast<float>(texSize.x),
                static_cast<float>(texSize.y)
            );
        }

        // Éviter division par zéro
        if (baseSize.x == 0.f || baseSize.y == 0.f)
            return { 0.f, 0.f };

        // Scale absolu (size / textureSize)
        sf::Vector2f absoluteScale = {
            cmd.size.x / baseSize.x,
            cmd.size.y / baseSize.y
        };

        // Scale final = scale absolu * scale SFML
        return {
            absoluteScale.x * cmd.scale.x,
            absoluteScale.y * cmd.scale.y
        };
    }

}