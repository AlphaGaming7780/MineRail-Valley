#pragma once
#include <SFML/Graphics.hpp>
#include <EnTT/entt.hpp>

#include <PallasEngine/Utils/Time.hpp>

#include <PallasEngine/ECS/Systems/SystemBase.hpp>
#include <PallasEngine/ECS/World.hpp>
#include <PallasEngine/ECS/Components/Animated.hpp>

#include <PallasEngine/Rendering/GameWindow.hpp>

namespace pallas
{
    class RenderSystem : public SystemBase
    {
    public:
        RenderSystem(World* world, entt::registry* registry, Logger& logger)
            : SystemBase(world, registry, logger), m_Window(&GameWindow::Instance().Get())
        {
        }

        UpdatePhase Phase() const override { return UpdatePhase::Rendering; }

        void Update() override;

    private:
        struct DrawItem
        {
            entt::entity entity;
            int zIndex;
        };

        std::vector<DrawItem> m_DrawList;
        sf::RenderWindow* m_Window = nullptr;

        void ApplyAnimation(sf::Sprite& sprite, Animated& anim, sf::Texture* texture);
    };
}