#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	class GameObject
	{
	public:
		GameObject() = default;
		~GameObject() = default;

		sf::Vector2f m_Position = { 0.f, 0.f };
		sf::Vector2f m_Origin = { 0.f, 0.f };
		sf::Vector2f m_Size = { 1.f, 1.f };
		sf::Vector2f m_Scale = { 1.f, 1.f };
		sf::Angle    m_Rotation = sf::Angle::Zero;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		virtual void OnCreate() {};
		virtual void Update() {};
		virtual void OnDestroy() {};
		virtual void Draw() {};


	private:
		
	};
}