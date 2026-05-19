#pragma once

#include <Game/GameObjects/GameObject.h>

namespace Game
{
	class TrackObject : public GameObject
	{
	public:
		// Représente la connection avce les autres tracks
		TrackObject* m_First;
		TrackObject* m_Second;

	};
}