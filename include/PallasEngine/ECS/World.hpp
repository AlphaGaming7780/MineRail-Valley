#pragma once

#include <SFML/Graphics.hpp>
#include <EnTT/entt.hpp>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <PallasEngine/Logging/Logger.hpp>

//#include <PallasEngine/ECS/Systems/SystemBase.hpp>
#include <PallasEngine/ECS/UpdatePhase.hpp>
#include <PallasEngine/ECS/Dependencies.hpp>
#include <PallasEngine/ECS/Components.hpp>
#include <PallasEngine/Utils/EcsComponents.hpp>

namespace pallas {

    class SystemBase;

    template<typename T, typename = void>
    struct has_after : std::false_type {};

    template<typename T>
    struct has_after<T, std::void_t<typename T::After>> : std::true_type {};

    template<typename T, typename = void>
    struct has_before : std::false_type {};

    template<typename T>
    struct has_before<T, std::void_t<typename T::Before>> : std::true_type {};

    class World {
    public:

        static World& Instance();

        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        template<typename TSystemBase>
        TSystemBase* GetOrCreateSystem();

        template<typename TComponent>
        entt::entity GetOrCreateSingletonEntity();
        
        void Pause(bool paused = true) { m_Paused = paused; }
        void UnPause() { Pause(false); }

        void Update();

        void Shutdown();

        entt::registry m_Registry;

    private:

        World() = default;

        Logger m_Logger = Logger("ECS_World");

        bool m_Paused = false;

        std::unordered_map<std::type_index, SystemBase*> m_Systems;
        std::unordered_map<UpdatePhase, std::vector<SystemBase*>> m_SystemsByPhase;

        const std::array<UpdatePhase, 7> m_PhaseOrder = {
            UpdatePhase::Begin,
            UpdatePhase::Update,
            UpdatePhase::Physics,
            UpdatePhase::LateUpdate,
            UpdatePhase::UI,
            UpdatePhase::Rendering,
            UpdatePhase::End
        };

        void SortSystemsForPhase(UpdatePhase phase);

        template<typename TSystemBase>
        auto GetAfterDependencies();

        template<typename TSystemBase>
        auto GetBeforeDependencies();
    };

    template<typename TSystemBase>
    TSystemBase* World::GetOrCreateSystem()
    {
        static_assert(std::is_base_of_v<SystemBase, TSystemBase>,
            "TSystemBase must inherit from SystemBase");

        std::type_index type = typeid(TSystemBase);

        auto it = m_Systems.find(type);
        if (it != m_Systems.end()) {
            return static_cast<TSystemBase*>(it->second);
        }

        // IMPORTANT : passer le registry au constructeur
        TSystemBase* instance = new TSystemBase(this, &m_Registry, m_Logger);

        m_SystemsByPhase[instance->Phase()].push_back(instance);
        m_Systems.emplace(type, instance);

        SortSystemsForPhase(instance->Phase());

        instance->OnCreate();

        return instance;
    }

    template<typename TComponent>
    entt::entity World::GetOrCreateSingletonEntity()
    {
        auto view = m_Registry.view<TComponent>();

        if (!view.empty())
        {
            if (view.size() > 1)
            {
                m_Logger.WarnO("Multiple singleton entities found for component '", typeid(TComponent).name(), "'. Using the first one.");
            }

            return *view.begin();
        }

        entt::entity entity = m_Registry.create();
        m_Registry.emplace<TComponent>(entity);

        m_Logger.InfoO("Created singleton entity for component '", typeid(TComponent).name(), "'.");

        return entity;
    }

    template<typename TSystemBase>
    auto World::GetAfterDependencies()
    {
        static_assert(std::is_base_of_v<SystemBase, TSystemBase>,
            "TSystemBase must inherit from SystemBase");

        if constexpr (has_after<TSystemBase>::value) {
            return typename TSystemBase::After{};
        }
        else {
            return AfterDependencies<>{};
        }
    }

    template<typename TSystemBase>
    auto World::GetBeforeDependencies()
    {
        static_assert(std::is_base_of_v<SystemBase, TSystemBase>,
            "TSystemBase must inherit from SystemBase");

        if constexpr (has_before<TSystemBase>::value) {
            return typename TSystemBase::Before{};
        }
        else {
            return BeforeDependencies<>{};
        }
    }
}