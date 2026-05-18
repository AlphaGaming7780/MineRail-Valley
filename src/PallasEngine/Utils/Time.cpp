#include <PallasEngine/Utils/Time.hpp>

namespace pallas {

	sf::Clock Time::m_MainClock;
	sf::Time Time::m_DeltaTime;

	void Time::Update()
	{
		m_DeltaTime = m_MainClock.restart();
	}
	sf::Time Time::GetDeltaTime()
	{
		return m_DeltaTime;
	}
	void Time::Reset()
	{
		m_MainClock.restart();
		m_DeltaTime = sf::Time::Zero;
	}
}
