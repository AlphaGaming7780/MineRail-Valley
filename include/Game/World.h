#pragma once

#include <SFML/Graphics.hpp>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <PallasEngine/Logging/Logger.hpp>

#include <Game/Systems/SystemBase.hpp>
#include <Game/Events.h>
#include <Game/GameObjects/GameObject.h>

namespace Game {

    class SystemBase;

    class World : public IEventObserver<LoadingStart>
    {
    public:

        static World& Instance() {
            static World inst;
            return inst;
        }

        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        template<typename TSystemBase>
        TSystemBase* GetOrCreateSystem();

        template<typename TGameObject, typename... Args>
        TGameObject* CreateGameObject(Args&&... args);

        void DestroyObject(GameObject* gameObject);

        const std::vector<GameObject*>& GetAllGameObject() const { return m_GameObjects; }
        
        template<typename TGameObject>
        std::vector<TGameObject*> GetAllGameObject() const;

        void Pause(bool paused = true);
        void UnPause() { Pause(false); }
        bool IsPaused() const { return m_Paused; }

        void Update();

        void Shutdown();

        void OnEvent(const LoadingStart& ev);

    private:

        World()
        {
            EventManager::Instance().Register<LoadingStart>(this);
        }

        pallas::Logger m_Logger = pallas::Logger("World");

        bool m_Paused = false;

        std::unordered_map<std::type_index, SystemBase*> m_Systems;
        std::vector<GameObject*> m_GameObjects;

        void CreateMap(MapData* mapData);

        void ClearWorld();

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

    template<typename TGameObject, typename... Args>
    TGameObject* World::CreateGameObject(Args&&... args)
    {
        static_assert(std::is_base_of_v<GameObject, TGameObject>,
            "TGameObject must inherit from GameObject");

        TGameObject* instance = new TGameObject(std::forward<Args>(args)...);

        m_GameObjects.push_back(instance);

        instance->OnCreate();

        return instance;
    }

    template<typename TGameObject>
    std::vector<TGameObject*> World::GetAllGameObject() const
    {
        static_assert(std::is_base_of_v<GameObject, TGameObject>,
            "TGameObject must inherit from GameObject");

        std::vector<TGameObject*> result;
        result.reserve(m_GameObjects.size());

        for (GameObject* go : m_GameObjects)
        {
            if (auto casted = dynamic_cast<TGameObject*>(go))
                result.push_back(casted);
        }

        return result;
    }

}