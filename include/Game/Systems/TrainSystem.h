#pragma once

#include <Game/Systems/SystemBase.hpp>

namespace Game
{
	class TrainSystem : public SystemBase
	{
	public:
		TrainSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		void OnUpdate() override {}
	};
}