
#include <PallasEngine/Logging.hpp>
#include <Game/AssetDatabase/AssetType/MapData.h>
#include <Game/AssetDatabase/TileDatabase.h>

#include <Game/World.h>

#include <Game/GameObjects/Tiles/TileObject.h>

namespace Game
{
    World::~World()
    {
        Shutdown();
    }

    void World::DestroyObject(GameObject* gameObject)
    {
        // Trouver l'objet dans le vector
        auto it = std::find(m_GameObjects.begin(), m_GameObjects.end(), gameObject);

        if (it == m_GameObjects.end())
        {
            m_Logger.ErrorO("[DestroyObject] GameObject isn't in the vector.");
            delete gameObject;
            return;
        }

        gameObject->OnDestroy();
        m_GameObjects.erase(it);
        delete gameObject;
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
    void World::OnEvent(const LoadingStart& ev)
    {
        Purpose purpose = ev.m_Purpose;
        GameMode gameMode = ev.m_GameMode;
        MapData* mapData = ev.m_MapData;

        if (purpose == Purpose::NewGame)
        {
            CreateMap(mapData);
        }
        else if (purpose == Purpose::Cleanup)
        {
            ClearWorld();
        }

    }

    void World::CreateMap(MapData* mapData)
    {
        int mapEdgeSize = mapData->MapSize;

        for (int i = 0; i < mapData->tiles.size(); ++i)
        {
            const TileData& t = *TileDatabase::Instance().Load(mapData->tiles[i]);

            int xIndex = i % mapEdgeSize;
            int yIndex = i / mapEdgeSize;

            float x = xIndex * t.object.m_Size.x;
            float y = yIndex * t.object.m_Size.y;

            TileObject* to = CreateGameObject<TileObject>(t, sf::Vector2i(xIndex, yIndex));
            to->m_RenderLayer = RenderLayer::Terrain;
        }
    }

    void World::ClearWorld()
    {
        for (GameObject* go : m_GameObjects)
        {
            if (!go) continue;
            go->OnDestroy();
            delete go;
        }

        m_GameObjects.clear();
    }
}