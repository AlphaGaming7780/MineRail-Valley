#include <PallasEngine/ECS/Systems/RenderSystems/RenderSystemBase.hpp>
#include <PallasEngine/Rendering/RenderingManager.hpp>

namespace pallas
{
    RenderSystemBase::RenderSystemBase(World* world, entt::registry* registry, Logger& logger)
        : SystemBase(world, registry, logger)
    {
        // Enregistrement automatique auprès du RenderingManager
        RenderingManager::Instance().RegisterQueue(this);
    }
}