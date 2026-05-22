#pragma once
#include<vector>
#include<string>


namespace Game
{
	struct MapData
	{
		// Width of the map in tiles. Maps are now allowed to be rectangular: if MapHeight == 0,
		// the map is square (legacy behaviour) and MapHeight is assumed equal to MapSize.
		int MapSize = 0;
		int MapHeight = 0;
		int Difficulty = 0;

		// Background music. BgPlaylist points to a folder under assets/ (e.g. "Musics\\1.Night_Drive").
		// All audio files in that folder will be shuffled and played back-to-back at load time.
		// BgAudioPath is kept for backward-compat with old maps that referenced a single file; it is
		// only used when BgPlaylist is empty.
		std::string BgPlaylist;
		std::string BgAudioPath;

		// Optional biome tag (e.g. "Plain", "Frozen", "Volcanic", "Forest", "Desert", "Neon").
		// Purely informational metadata for now — used by the audio/UI layer for theming.
		std::string Biome;

		std::string MapPreview;

		std::vector<std::string> tiles;
	};
}