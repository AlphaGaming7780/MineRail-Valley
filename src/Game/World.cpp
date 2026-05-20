
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
        if (gameObject == nullptr) return;

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
            sys->OnUpdate();
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
        int totalTiles = mapData->tiles.size();

        // Tableau dynamique pour stocker les pointeurs vers les tiles
        std::vector<TileObject*> tiles(totalTiles, nullptr);

        for (int i = 0; i < totalTiles; ++i)
        {
            const TileData& t = *TileDatabase::Instance().Load(mapData->tiles[i]);

            int xIndex = i % mapEdgeSize;
            int yIndex = i / mapEdgeSize;

            float x = xIndex * t.object.m_Size.x;
            float y = yIndex * t.object.m_Size.y;

            TileObject* to = CreateGameObject<TileObject>(t, sf::Vector2i(xIndex, yIndex));
            to->m_RenderLayer = RenderLayer::Terrain;
            if(!t.object.m_AnimSync) to->m_AnimCurrentTime = static_cast<float>(rand()) / RAND_MAX * to->m_AnimDuration;

            tiles[i] = to;

            int upIdx = i - mapEdgeSize;
            if (upIdx >= 0)
            {
                TileObject* upTile = tiles[upIdx];
                if (upTile)
                {
                    upTile->m_Down = to;
                    to->m_Up = upTile;
                }
            }

            int leftIdx = i - 1;
            if (leftIdx >= 0 && (i % mapEdgeSize != 0))
            {
                TileObject* leftTile = tiles[leftIdx];
                if (leftTile)
                {
                    leftTile->m_Right = to;
                    to->m_Left = leftTile;
                }
            }
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