#pragma once
#include<Game/GameObjects/GameObject.h>

namespace Game
{
	class TrainObject;

	class WagonObject : public GameObject
	{
	public:
		TrainObject* Train = nullptr;
	};
}