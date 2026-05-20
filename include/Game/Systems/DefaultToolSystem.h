#pragma once

#include <Game/Systems/SystemBase.hpp>

namespace Game
{
	class DefaultToolSystem : public SystemBase
	{
	public:
		DefaultToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		void Update() override {}
	};
}