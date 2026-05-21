#pragma once

#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/Rendering/Camera.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/World.h>

#include <queue>
#include <unordered_map>
#include <algorithm> // std::reverse

namespace Game
{
	class TrackUtils
	{
	public:

		static std::vector<TrackObject*> GetPath(
			TrackObject* startTrack,
			TrackObject* endTrack
			)
		{
			std::queue<TrackObject*> q;
			std::unordered_map<TrackObject*, TrackObject*> parent;

			q.push(startTrack);
			parent[startTrack] = nullptr;


			while (!q.empty())
			{
				TrackObject* current = q.front();
				q.pop();

				if (current == endTrack)
					break;

				for (TrackObject* neighbour : current->GetAdjacentTrackObjects())
				{
					if (parent.find(neighbour) == parent.end())
					{
						parent[neighbour] = current;
						q.push(neighbour);
					}
				}
			}

			// Aucun chemin trouvé
			if (parent.find(endTrack) == parent.end())
				return {};

			std::vector<TrackObject*> path;
			TrackObject* step = endTrack;

			while (step != nullptr) 
			{
				path.push_back(step);
				step = parent[step];
			}

			std::reverse(path.begin(), path.end());
			return path;
		}
	};
}