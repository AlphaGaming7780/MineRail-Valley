#pragma once
#include <SFML/System.hpp>
namespace Game 
{
	class Time
	{
	public:
		static void Update();
		static sf::Time GetDeltaTime();

	private:
		static sf::Clock m_MainClock;
		static sf::Time m_DeltaTime;

		friend class GameManager;
		static void Reset();
	};
}
