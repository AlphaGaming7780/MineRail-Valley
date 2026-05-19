#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <Game/Events.h>
#include <Game/AssetDatabase.h>
#include <string>
#include <map>

namespace Game
{
	class AudioManager : public IEventObserver<AudioEvent>
	{
	public:
		AudioManager();
		~AudioManager();

		void OnEvent(const AudioEvent& event) override;

		void PlaySound(std::string& path, float volume = 1.0f);
		void PlayMusic(std::string& path, bool loop = false, float volume = 1.0f);

		void PauseMusic(std::string& path);
		void ResumeMusic(std::string& path);
		void StopMusic(std::string& path);

		void SetMasterVolume(float volume);
		void SetSoundVolume(float volume);
		void SetMusicVolume(float volume);

		void UnloadAll();

		void Update();

	private:
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