
#include <PallasEngine/Logging.hpp>

#include <Game/World.h>

namespace Game
{
    World::~World()
    {
        Shutdown();
    }

    void World::Update()
    {
        if (m_Paused) return;
        for (auto& [type, sys] : m_Systems) 
        {
            if (!sys->IsEnabled()) continue;
#ifdef _DEBUG
            pallas::PerformanceLogger::Scoped perf(typeid(*sys).name());
#endif
            sys->Update();
        }

        for (GameObject* go : m_GameObjects)
        {
            if (!go->m_Enabled) continue;
            go->Update();
        }
    }

    void World::Shutdown()
    {
        for (GameObject* go : m_GameObjects) 
        {
            go->OnDestroy();
            delete go;
        }

        for (auto& pair : m_Systems) 
        {
            pair.second->OnDestroy();
            delete pair.second;
        }

        m_Systems.clear();
        m_GameObjects.clear();
    }
}