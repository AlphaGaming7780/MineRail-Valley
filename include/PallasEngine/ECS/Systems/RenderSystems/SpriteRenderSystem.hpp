#pragma once
#include <PallasEngine/ECS/Systems/RenderSystems/RenderSystemBase.hpp>

namespace pallas
{
    class SpriteRenderSystem : public RenderSystemBase
    {
    public:
        SpriteRenderSystem(World* world, entt::registry* registry, Logger& logger)
            : RenderSystemBase(world, registry, logger)
        {
        }

        virtual void Update() override;

    private:

    };
}