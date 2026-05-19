#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	class GameObject
	{
	public:
		
		~GameObject() = default;

		sf::Vector2f m_Position = { 0.f, 0.f };
		sf::Vector2f m_Origin = { 0.f, 0.f };
		sf::Vector2f m_Size = { 1.f, 1.f };
		sf::Vector2f m_Scale = { 1.f, 1.f };
		sf::Angle    m_Rotation = sf::Angle::Zero;

		sf::Texture* m_Texture = nullptr;

		bool m_Enabled = false;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		virtual void OnCreate() {};
		virtual void Update() {};
		virtual void OnDestroy() {};
		virtual void Draw() {};

	protected:
		friend class World;
		GameObject() = default;
	};
}