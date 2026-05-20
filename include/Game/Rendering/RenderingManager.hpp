#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/RenderLayer.h>
#include <Game/GameObjects/GameObject.h>

namespace Game
{

    class RenderingManager
    {
    public:

        static RenderingManager& Instance();

        void Render(sf::RenderWindow& window);

        Camera& GetCamera() { return m_Camera; }

        void NewFrame();

    private:
        RenderingManager() = default;

        pallas::Logger m_Logger = pallas::Logger("RenderingManager");

        Camera m_Camera;
        std::unordered_map<RenderLayer, std::vector<GameObject*>> m_RenderList;
    };

}