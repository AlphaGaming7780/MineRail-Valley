#pragma once

#include <EnTT/entt.hpp>
#include <PallasEngine/ECS/World.hpp>
#include <PallasEngine/ECS/UpdatePhase.hpp>
#include <PallasEngine/ECS/Dependencies.hpp>
#include <PallasEngine/Logging/Logger.hpp>

namespace pallas
{
    class World;

    class SystemBase
    {
    public:
        SystemBase() = default;

        explicit SystemBase(World* world, entt::registry* registry, Logger& logger)
            : m_World(world), m_Registry(registry), m_Logger(logger)
        {
        }

        virtual ~SystemBase() = default;

        virtual void OnCreate() {};
        virtual void Update() = 0;
        virtual void OnDestroy() {};

        virtual UpdatePhase Phase() const = 0;

        void SetEnable(bool enable);
        bool IsEnabled() const;
        virtual void OnEnabled(bool enabled) {};

        entt::entity CreateEntity();

        template<typename TComponent>
        entt::entity GetOrCreateSingletonEntity();


    protected:
        World* m_World;
        entt::registry* m_Registry = nullptr;
        Logger m_Logger;

    private:
        bool m_Enabled = true;

    };

    template<typename TComponent>
    entt::entity SystemBase::GetOrCreateSingletonEntity()
    {
        return m_World->GetOrCreateSingletonEntity<TComponent>();
    }

}