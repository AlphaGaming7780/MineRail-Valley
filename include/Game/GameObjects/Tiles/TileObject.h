#pragma once
#include <Game/GameObjects/GameObject.h>
#include <Game/AssetDatabase/AssetType/TileData.h>

namespace Game
{
	class TileObject : public GameObject
	{
		friend class World;
	public:
		sf::Vector2i m_Index = sf::Vector2i(-1, -1);
		bool m_CanBuild = false;

	protected:
		TileObject() : GameObject() {}
		TileObject(const TileData& tileData, const sf::Vector2i& index) : GameObject(tileData.object) 
		{
			m_Index = index;
			m_CanBuild = tileData.m_CanBuild;

			float x = m_Index.x * m_Size.x;
			float y = m_Index.y * m_Size.y;

			m_Sprite->setPosition(sf::Vector2f(x, y));
			m_Enabled = true;
		}
	};
}