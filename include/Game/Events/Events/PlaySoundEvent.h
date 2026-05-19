#pragma once

#include <SFML/Window/Event.hpp>

namespace Game
{
	struct PlaySoundEvent
	{
		std::string audioPath;
		float volume = 100.0f;
	};
}