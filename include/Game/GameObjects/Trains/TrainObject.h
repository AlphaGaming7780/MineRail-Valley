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
		TrackObject* Next;
		TrackObject* Current;
		TrackObject* Previous;

		float speed;

		StationObject* StationSrc;
		StationObject* StationDest;

		std::vector<WagonObject*> Wagons;
	};
}