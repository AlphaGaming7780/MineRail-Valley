#pragma once

#include <SFML/Graphics.hpp>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <PallasEngine/Logging/Logger.hpp>

#include <Game/Systems/SystemBase.hpp>
#include <Game/Events/IEventObserver.hpp>
#include <Game/GameObjects/GameObject.h>

namespace Game {

    class SystemBase;

    class World
    {
    public:

        static World& Instance();

        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        template<typename TSystemBase>
        TSystemBase* GetOrCreateSystem();

        template<typename TGameObject>
        TGameObject* CreateGameObject();

        void Pause(bool paused = true) { m_Paused = paused; }
        void UnPause() { Pause(false); }

        void Update();

        void Shutdown();

    private:

        World() = default;

        pallas::Logger m_Logger = pallas::Logger("World");

        bool m_Paused = false;

        std::unordered_map<std::type_index, SystemBase*> m_Systems;
        std::vector<GameObject*> m_GameObjects;
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

        TSystemBase* instance = new TSystemBase(this, m_Logger);

        m_Systems.emplace(type, instance);

        instance->OnCreate();

        return instance;
    }
    template<typename TGameObject>
    inline TGameObject* World::CreateGameObject()
    {
        static_assert(std::is_base_of_v<GameObject, TGameObject>,
            "TGameObject must inherit from GameObject");

        TGameObject* instance = new TGameObject();

        m_GameObjects.push_back(instance);

        instance->OnCreate();

        return instance;
    }
}