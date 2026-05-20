#pragma once

#include<string>

namespace Game
{
	struct PlaySoundEvent
	{
		std::string Path;
		float volume = 100.0f;
	};
}