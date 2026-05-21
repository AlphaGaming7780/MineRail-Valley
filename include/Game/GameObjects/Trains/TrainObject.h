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
		friend class World;
	public:
		TrackObjectBase* m_Next = nullptr;
		TrackObjectBase* m_Current = nullptr;
		TrackObjectBase* m_Previous = nullptr;

		float m_Speed = 0.f;

		StationObject* m_StationSrc = nullptr;
		StationObject* m_StationDest = nullptr;

		std::vector<WagonObject*> m_Wagons;


		void Update() override;

	protected:
		TrainObject() : GameObject() { m_RenderLayer = RenderLayer::Trains; }
	};
}