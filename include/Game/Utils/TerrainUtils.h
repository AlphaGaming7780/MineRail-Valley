#pragma once

#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/World.h>
#include <queue>

namespace Game
{
    class TerrainUtils
    {
    public:

        static std::vector<TileObject*> GetPath(World& world, TileObject* startTile, TileObject* endTile, bool canBuild = true)
        {
            return GetPath(world, startTile->m_Index, endTile->m_Index, canBuild);
        }

        static std::vector<TileObject*> GetPath(
            World& world,
            const sf::Vector2i& startIndex,
            const sf::Vector2i& endIndex,
            bool canBuild = true)
        {
            std::vector<TileObject*> result;

            // Récupération de toutes les tiles
            const auto tiles = world.GetAllGameObject<TileObject>();

            TileObject* start = nullptr;
            TileObject* goal = nullptr;

            for (TileObject* t : tiles)
            {
                if (t->m_Index == startIndex) start = t;
                if (t->m_Index == endIndex)   goal = t;
            }

            if (!start || !goal)
                return result; 

            // BFS
            std::queue<TileObject*> q;
            std::unordered_map<TileObject*, TileObject*> parent;

            q.push(start);
            parent[start] = nullptr;

            auto tryPush = [&](TileObject* from, TileObject* to)
                {
                    if (!to) return;
                    if (parent.contains(to)) return;

                    if (canBuild && !to->m_CanBuild)
                        return;

                    parent[to] = from;
                    q.push(to);
                };

            while (!q.empty())
            {
                TileObject* cur = q.front();
                q.pop();

                if (cur == goal)
                    break;

                tryPush(cur, cur->m_Up);
                tryPush(cur, cur->m_Down);
                tryPush(cur, cur->m_Left);
                tryPush(cur, cur->m_Right);
            }

            if (!parent.contains(goal))
                return result;

            TileObject* cur = goal;
            while (cur)
            {
                result.push_back(cur);
                cur = parent[cur];
            }

            std::reverse(result.begin(), result.end());
            return result;
        }

    };
}
