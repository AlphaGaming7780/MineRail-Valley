#pragma once
#include<Game/GameObjects/GameObject.h>
#include<Game/GameObjects/Trains/TrainObject.h>

namespace Game
{
	class WagonObject : public GameObject
	{
	public:
		TrainObject* Train;
	};
}