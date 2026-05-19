#pragma once

#include <Game/GameObjects/GameObject.h>

namespace Game
{
	class TrackObject : GameObject
	{
		// Représente la connection avce les autres tracks
		TrackObject* m_First;
		TrackObject* m_Second;

	};
}