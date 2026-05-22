#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <Game/Events.h>
#include <Game/AssetDatabase.h>
#include <string>
#include <map>

namespace Game
{
	class AudioManager 
		: public IEventObserver<PlaySoundEvent>
		, public IEventObserver<PlayMusicEvent>
		, public IEventObserver<PauseEvent>
		, public IEventObserver<LoadingStart>
		, public IEventObserver<LoadingComplete>
	{
	public:
		~AudioManager();

		// Singleton
		static AudioManager& Instance() {
			static AudioManager inst;
			return inst;
		}

		void OnEvent(const PlaySoundEvent& event) override;
		void OnEvent(const PlayMusicEvent& event) override;
		void OnEvent(const PauseEvent& event) override; 
		void OnEvent(const LoadingStart& event) override;
		void OnEvent(const LoadingComplete& event) override;

		void PlaySound(const std::string& path, float volume = 1.0f);
		void PlayMusic(const std::string& path, bool loop = false, float volume = 1.0f);

		// Playlist API. `folder` is a path under assets/ (e.g. "Musics\\1.Night_Drive").
		// All audio files in that folder are shuffled and played back-to-back; when the last track
		// finishes the playlist re-shuffles and starts over. Calling PlayPlaylist while another
		// playlist is active stops the previous one.
		void PlayPlaylist(const std::string& folder, float volume = 1.0f);
		void StopPlaylist();

		void PauseMusic(const std::string& path);
		void ResumeMusic(const std::string& path);
		void StopMusic(const std::string& path);

		void PauseAllSounds();
		void ResumeAllSounds();

		void SetMasterVolume(float volume);
		void SetSoundVolume(float volume);
		void SetMusicVolume(float volume);

		void StopAll();
		void UnloadAll();

		void Update();

	private:
		AudioManager();

		void _PurgedFinishedSounds();
		void _UpdateVolume();
		void _AdvancePlaylist();
		void _ResetPlaylist();

		SoundDatabase& m_SoundDatabase;
		MusicDatabase& m_MusicDatabase;

		std::vector<sf::Sound*> m_PlayingSounds;

		float m_MasterVolume;
		float m_SoundVolume;
		float m_MusicVolume;

		// Playlist state. Empty m_PlaylistFolder means no playlist is currently active.
		std::vector<std::string> m_PlaylistTracks;   // shuffled order, paths relative to assets/
		std::size_t              m_PlaylistIndex = 0;
		std::string              m_PlaylistFolder;
		float                    m_PlaylistVolume = 0.f;
		sf::Music*               m_PlaylistCurrent = nullptr;
	};
}