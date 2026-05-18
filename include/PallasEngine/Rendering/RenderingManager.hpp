#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <PallasEngine/Rendering/RenderCommand.hpp>
#include <PallasEngine/Rendering/Camera.hpp>
#include <PallasEngine/ECS/Systems/RenderSystems/RenderSystemBase.hpp>
#include <PallasEngine/ECS/Components/CameraComponent.hpp>

namespace pallas
{
    class RenderingManager
    {
    public:

        //struct CameraState
        //{
        //    sf::Vector2f position;
        //    float        zoom = 1.f;
        //    sf::Angle    rotation = sf::Angle::Zero;
        //    sf::Vector2f size = { 0.f, 0.f };
        //    sf::Vector2f baseSize = { 0.f, 0.f };
        //};


        static RenderingManager& Instance();

        void RegisterQueue(RenderSystemBase* queue);

        void Render(sf::RenderWindow& window);

        Camera& GetCamera() { return m_Camera; }
        const CameraComponent GetCameraState() const;
        void SetCameraState(const CameraComponent& state);

    private:
        RenderingManager() = default;

        Camera m_Camera;
        CameraComponent m_CameraState;
        std::vector<RenderSystemBase*> m_Queues;
        std::vector<RenderCommand> m_Merged; // buffer temporaire pour fusionner

        sf::Vector2f GetActualScale(const RenderCommand& cmd);
    };

}