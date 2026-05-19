#pragma once
#include<Game/GameObjects/GameObject.h>

namespace Game
{
	class TileObject : public GameObject
	{
	public:
		bool CanBuild = false;
	};
}