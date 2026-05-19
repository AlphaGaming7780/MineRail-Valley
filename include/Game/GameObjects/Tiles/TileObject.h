#pragma once
#include<Game/GameObjects/GameObject.h>

namespace Game
{
	class TileObject : public GameObject
	{
	public:
		sf::Vector2i m_Index;
		bool CanBuild = false;
	};
}