#pragma once
#include<Game/GameObjects/GameObject.h>
#include<Game/GameObjects/Tracks/TrackObject.h>
#include<Game/GameObjects/Tracks/StationObject.h>
#include<Game/GameObjects/Wagons/WagonObject.h>
#include<vector>

namespace Game
{
	class TrainObject : public GameObject
	{
	public:
		TrackObject* Next = nullptr;
		TrackObject* Current = nullptr;
		TrackObject* Previous = nullptr;

		float speed = 0.f;

		StationObject* StationSrc = nullptr;
		StationObject* StationDest = nullptr;

		std::vector<WagonObject*> Wagons;
	};
}