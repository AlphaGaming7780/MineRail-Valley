
#include <PallasEngine/Logging.hpp>
#include <Game/AssetDatabase/AssetType/MapAsset.h>

#include <Game/World.h>

#include <Game/GameObjects/Tiles/TileObject.h>

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
    void World::OnEvent(const LoadingStart& ev)
    {
        Purpose purpose = ev.m_Purpose;
        GameMode gameMode = ev.m_GameMode;
        MapData* mapData = ev.m_MapAsset;

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
        const float tileSize = 50.f;
        int mapEdgeSize = mapData->MapSize;

        for (int i = 0; i < mapData->tiles.size(); ++i)
        {
            TileData* t = mapData->tiles[i];

            int xIndex = i % mapEdgeSize;
            int yIndex = i / mapEdgeSize;

            float X = xIndex * tileSize;
            float Y = yIndex * tileSize;

            TileObject* to = CreateGameObject<TileObject>();

            to->m_Index = sf::Vector2i(xIndex, yIndex);
            to->m_Position = sf::Vector2f(X, Y);
            to->m_Size = sf::Vector2f(tileSize, tileSize);
            to->CanBuild = t->CanBuild;
            to->m_Texture = t->TextureAsset;
        }
    }

    void World::ClearWorld()
    {
    }
}