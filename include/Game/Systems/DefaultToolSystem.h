#pragma once

#include <SFML/Graphics.hpp>

#include <Game/Systems/SystemBase.hpp>

namespace Game
{
	class DefaultToolSystem : public SystemBase
	{
	public:

		static const sf::Color kHoverColor;
		static const sf::Color kSelectColor;

		DefaultToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}
		
		void OnUpdate() override {}
	};

	inline constexpr sf::Color DefaultToolSystem::kHoverColor(0, 255, 255, 64);
	inline constexpr sf::Color DefaultToolSystem::kSelectColor(0, 255, 255, 128);

}