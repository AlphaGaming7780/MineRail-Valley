#pragma once

#include <Game/GameObjects/GameObject.h>
#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/World.h>

namespace Game
{
	class RaycastUtils
	{
	public:
		static GameObject* PerformRaycast(const World& world, const sf::RenderWindow& window)
		{
            if (UIManager::Instance().IsMouseOverUI()) return nullptr;

			const std::vector<GameObject*>& gameObjects = world.GetAllGameObject();

			sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

			GameObject* result = nullptr;

			for (GameObject* go : gameObjects)
			{
                if (!go->GetGlobalBounds().contains(worldPos))
                    continue;

                if (!result)
                {
                    result = go;
                    continue;
                }

                if (go->m_RenderLayer > result->m_RenderLayer)
                {
                    result = go;
                    continue;
                }

                if (go->m_RenderLayer == result->m_RenderLayer &&
                    go->zIndex > result->zIndex)
                {
                    result = go;
                }
			}
			return result;
		}

        template<typename T>
        static T* PerformRaycast(const World& world, const sf::RenderWindow& window)
        {
            if (UIManager::Instance().IsMouseOverUI()) return nullptr;

            const auto gameObjects = world.GetAllGameObject<T>();

            sf::Vector2f worldPos = InputManager::Instance().GetMouseWorldPos();

            T* result = nullptr;

            for (T* go : gameObjects)
            {
                if (!go->GetGlobalBounds().contains(worldPos))
                    continue;

                if (!result)
                {
                    result = go;
                    continue;
                }

                if (go->m_RenderLayer > result->m_RenderLayer)
                {
                    result = go;
                    continue;
                }

                if (go->m_RenderLayer == result->m_RenderLayer &&
                    go->zIndex > result->zIndex)
                {
                    result = go;
                }
            }

            return result;
        }

	};
}
