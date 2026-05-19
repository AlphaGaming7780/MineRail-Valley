#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <Game/Rendering/RenderCommand.hpp>
#include <Game/Rendering/Camera.hpp>

namespace Game
{
    class RenderingManager
    {
    public:

        static RenderingManager& Instance();

        void Render(sf::RenderWindow& window);

        Camera& GetCamera() { return m_Camera; }

    private:
        RenderingManager() = default;

        Camera m_Camera;
        std::vector<RenderCommand> m_Merged; // buffer temporaire pour fusionner

        sf::Vector2f GetActualScale(const RenderCommand& cmd);
    };

}