#pragma once

#include <SFML/Window/Event.hpp>

namespace Game
{
	struct PlayMusicEvent
	{
		std::string& path;
		bool loop = false;
		float volume = 100.0f;
	};
}