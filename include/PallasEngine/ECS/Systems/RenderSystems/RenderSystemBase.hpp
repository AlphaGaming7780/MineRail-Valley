#pragma once
#include <vector>
#include <span>

#include <PallasEngine/ECS/Systems/SystemBase.hpp>
#include <PallasEngine/Rendering/RenderCommand.hpp>

namespace pallas
{
    class RenderSystemBase : public SystemBase
    {
    public:
        RenderSystemBase(World* world, entt::registry* registry, Logger& logger);

        UpdatePhase Phase() const override { return UpdatePhase::Rendering; }

        // --- Interface pour RenderingManager ---
        std::span<const RenderCommand> GetCommands() const
        {
            return std::span<const RenderCommand>(m_Commands.data(), m_Commands.size());
        }
         
        void ClearCommands()
        {
            m_Commands.clear();
        }

        virtual int GetLayer() const { return 0; }

    protected:
        std::vector<RenderCommand> m_Commands;
    };
}