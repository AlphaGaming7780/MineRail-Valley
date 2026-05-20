#pragma once

#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/World.h>

#include <queue>
#include <unordered_map>
#include <algorithm> // std::reverse

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
            //pallas::Logger logger("SceneFlow");

            std::vector<TileObject*> result;

            const auto tiles = world.GetAllGameObject<TileObject>();
            //logger.InfoO("GetPath: tiles count = ", tiles.size());
            //logger.InfoO("GetPath: startIndex = ", startIndex, " , endIndex = ", endIndex);

            TileObject* start = nullptr;
            TileObject* goal = nullptr;

            for (TileObject* t : tiles)
            {
                if (t->m_Index == startIndex) start = t;
                if (t->m_Index == endIndex)   goal = t;
            }

            if (!start || !goal)
            {
                //logger.InfoO("GetPath: start or goal not found (start=", start, ", goal= ", goal,")");
                return result;
            }

            //logger.InfoO("GetPath: startTile canBuild=", start->m_CanBuild,", goalTile canBuild=", goal->m_CanBuild);

            std::queue<TileObject*> q;
            std::unordered_map<TileObject*, TileObject*> parent;

            q.push(start);
            parent[start] = nullptr;

            auto tryPush = [&](TileObject* from, TileObject* to)
                {
                    if (!to) return;
                    if (parent.contains(to)) return;

                    // On autorise toujours start/goal, même si non buildable
                    if (canBuild && !to->m_CanBuild && to != start && to != goal)
                    {
                        //logger.InfoO("GetPath: skip tile (", to->m_Index,") - not buildable");
                        return;
                    }

                    //logger.InfoO("GetPath: visit neighbor (", to->m_Index,") from (", from->m_Index,")");

                    parent[to] = from;
                    q.push(to);
                };

            int step = 0;

            while (!q.empty())
            {
                TileObject* cur = q.front();
                q.pop();

                //logger.InfoO("GetPath: BFS step ", step++, ", current=(", cur->m_Index,")");

                if (cur == goal)
                {
                    //logger.InfoO("GetPath: goal reached at (", cur->m_Index,")");
                    break;
                }

                tryPush(cur, cur->m_Up);
                tryPush(cur, cur->m_Down);
                tryPush(cur, cur->m_Left);
                tryPush(cur, cur->m_Right);
            }

            if (!parent.contains(goal))
            {
                //logger.InfoO("GetPath: no path found to goal.");
                return result;
            }

            TileObject* cur = goal;
            while (cur)
            {
                result.push_back(cur);
                cur = parent[cur];
            }

            std::reverse(result.begin(), result.end());

            //logger.InfoO("GetPath: path length = ", result.size());
            //for (auto* t : result)
            //{
            //    logger.InfoO("  -> (", t->m_Index,")");
            //}

            return result;
        }
    };
}
