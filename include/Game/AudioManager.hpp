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