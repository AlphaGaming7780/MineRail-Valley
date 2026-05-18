#include <Entt/entt.hpp>
#include <queue>

#include <PallasEngine/ECS/World.hpp>
#include <PallasEngine/ECS/Systems.hpp>
#include <PallasEngine/ECS/Dependencies.hpp>
#include <PallasEngine/Logging/PerformanceLogger.hpp>

using namespace entt;

namespace pallas 
{
    World& World::Instance()
    {
        static World instance;
        return instance;
    }

    World::~World()
	{
        Shutdown();
	}

    void World::Update()
	{
        if (m_Paused) return;
		for (auto phase : m_PhaseOrder) {
			auto& list = m_SystemsByPhase[phase];
			for (auto* sys : list) {
#ifdef _DEBUG
                PerformanceLogger::Scoped perf(typeid(*sys).name());
#endif
				sys->Update();
			}
		}
	}

    void World::Shutdown()
    {
        // 1. Appeler OnDestroy() dans l'ordre inverse des phases
        for (auto it = m_PhaseOrder.rbegin(); it != m_PhaseOrder.rend(); ++it) {
            UpdatePhase phase = *it;

            auto& vec = m_SystemsByPhase[phase];
            for (SystemBase* sys : vec) {
                if (sys) {
                    sys->OnDestroy();
                }
            }
        }

        // 2. Libérer la mémoire des systèmes
        for (auto& pair : m_Systems) {
            delete pair.second;
        }

        // 3. Nettoyer les conteneurs
        m_Systems.clear();
        m_SystemsByPhase.clear();
    }

    void World::SortSystemsForPhase(UpdatePhase phase)
    {
        auto& list = m_SystemsByPhase[phase];

        std::unordered_map<SystemBase*, std::vector<SystemBase*>> graph;
        std::unordered_map<SystemBase*, int> indegree;

        for (auto* sys : list) {
            graph[sys] = {};
            indegree[sys] = 0;
        }

        for (auto* sys : list) {

            using SysType = std::remove_pointer_t<decltype(sys)>;

            //
            // AFTER
            //
            auto after = GetAfterDependencies<SysType>();
            using AfterTypes = typename decltype(after)::types;

            std::apply([&](auto... deps) {
                (([&] {
                    for (auto* other : list) {
                        if (typeid(*other) == typeid(deps)) {
                            graph[other].push_back(sys);
                            indegree[sys]++;
                        }
                    }
                    }()), ...);
                }, AfterTypes{});

            //
            // BEFORE
            //
            auto before = GetBeforeDependencies<SysType>();
            using BeforeTypes = typename decltype(before)::types;

            std::apply([&](auto... deps) {
                (([&] {
                    for (auto* other : list) {
                        if (typeid(*other) == typeid(deps)) {
                            graph[sys].push_back(other);
                            indegree[other]++;
                        }
                    }
                    }()), ...);
                }, BeforeTypes{});
        }

        //
        // Tri topologique
        //
        std::vector<SystemBase*> sorted;
        std::queue<SystemBase*> q;

        for (auto& [sys, deg] : indegree)
            if (deg == 0)
                q.push(sys);

        while (!q.empty()) {
            auto* sys = q.front();
            q.pop();
            sorted.push_back(sys);

            for (auto* next : graph[sys]) {
                indegree[next]--;
                if (indegree[next] == 0)
                    q.push(next);
            }
        }

        if (sorted.size() != list.size()) {
            throw std::runtime_error("Cycle de dépendances détecté !");
        }

        list = sorted;
    }

}