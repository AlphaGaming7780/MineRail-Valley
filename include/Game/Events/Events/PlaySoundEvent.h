#pragma once

namespace Game
{
	struct PlaySoundEvent
	{
		std::string audioPath;
		float volume = 100.0f;
	};
}