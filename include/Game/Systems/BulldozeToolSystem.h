#pragma once

#include <Game/Systems/SystemBase.hpp>

namespace Game
{
	class BulldozeToolSystem : public SystemBase
	{
	public:
		BulldozeToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		void OnUpdate() override {}
	};
}