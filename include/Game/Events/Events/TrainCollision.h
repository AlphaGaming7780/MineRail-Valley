#pragma once

#include <Game/GameObjects/Trains/TrainObject.h>

namespace Game
{
	struct TrainCollision
	{
		TrainObject* m_TrainA;
		TrainObject* m_TrainB;
		sf::Vector2f m_Posittion;
	};
}