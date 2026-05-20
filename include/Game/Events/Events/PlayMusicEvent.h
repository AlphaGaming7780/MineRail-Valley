#pragma once

#include<string>

namespace Game
{
	struct PlayMusicEvent
	{
		std::string Path;
		bool loop = false;
		float volume = 100.0f;
	};
}