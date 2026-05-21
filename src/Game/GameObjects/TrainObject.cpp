#include <Game/GameObjects//Trains/TrainObject.h>
#include <Game/Utils/Time.hpp>

namespace Game
{

	void TrainObject::Update()
	{
        GameObject::Update();
        if (!m_Next) return;

        const float dt = Time::GetDeltaTime().asSeconds();
        const float step = m_Speed * dt;
        if (step <= 0.f) return; // pas de vitesse configurée

        sf::Vector2f targetPos;


	}

}
