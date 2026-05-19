#pragma once

#include<Game/GameObjects/Tracks/TrackObject.h>
#include<vector>

namespace Game
{
	class NodeTrack : public TrackObject
	{
	public:
		std::vector<TrackObject*> Connections;
	};
}
