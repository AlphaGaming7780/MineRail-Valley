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

<<<<<<< Updated upstream
=======
		// Playlist API. `folder` is a path under assets/ (e.g. "Musics\\1.Night_Drive").
		// All audio files in that folder are shuffled and played back-to-back; when the last track
		// finishes the playlist re-shuffles and starts over. Calling PlayPlaylist while another
		// playlist is active stops the previous one.
		void PlayPlaylist(const std::string& folder, float volume = 1.0f);
		void StopPlaylist();

		// Playlist transport — used by UIMusicPlayer wiring.
		// TogglePlaylistPause pauses the current track if playing, resumes it otherwise.
		// AdvancePlaylist plays the next track in the shuffled order (wraps + re-shuffles at end).
		// RewindPlaylist plays the previous track (wraps to the last on underflow).
		void TogglePlaylistPause();
		void AdvancePlaylist();
		void RewindPlaylist();

		// Playlist introspection — used by UIMusicPlayer / GameManager to refresh the widget.
		bool                 IsPlaylistActive()  const { return !m_PlaylistFolder.empty(); }
		bool                 IsPlaylistPlaying() const;
		bool                 IsPlaylistPaused()  const;
		const std::string&   GetCurrentPlaylistFolder() const { return m_PlaylistFolder; }
		std::string          GetCurrentTrackPath() const;       // "" if no track
		std::string          GetCurrentTrackName() const;       // pretty filename without extension
		sf::Time             GetCurrentTrackTime() const;       // current playback offset
		sf::Time             GetCurrentTrackDuration() const;   // total duration
		float                GetCurrentTrackPosition01() const; // time/duration clamped [0,1]

>>>>>>> Stashed changes
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

		SoundDatabase& m_SoundDatabase;
		MusicDatabase& m_MusicDatabase;

		std::vector<sf::Sound*> m_PlayingSounds;

		float m_MasterVolume;
		float m_SoundVolume;
		float m_MusicVolume;
	};
}