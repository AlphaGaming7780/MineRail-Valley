#pragma once
#include <Game/AudioManager.hpp>

namespace Game
{
	AudioManager::AudioManager()
		: m_SoundDatabase(SoundDatabase::Instance())
		, m_MusicDatabase(MusicDatabase::Instance())
		, m_MasterVolume(100.0f)
		, m_SoundVolume(100.0f)
		, m_MusicVolume(100.0f)
	{ }
	AudioManager::~AudioManager()
	{
		UnloadAll();
	}
	void AudioManager::OnEvent(const AudioEvent & event)
	{
	}
	void AudioManager::PlaySound(std::string& path, float volume)
	{
	}
	void AudioManager::PlayMusic(std::string& path, bool loop, float volume)
	{
	}
	void AudioManager::PauseMusic(std::string& path)
	{
	}
	void AudioManager::ResumeMusic(std::string& path)
	{
	}
	void AudioManager::StopMusic(std::string& path)
	{
	}
	void AudioManager::SetMasterVolume(float volume)
	{
	}
	void AudioManager::SetSoundVolume(float volume)
	{
	}
	void AudioManager::SetMusicVolume(float volume)
	{
	}
	void AudioManager::UnloadAll()
	{
		StopMusic();
		m_SoundDatabase.UnloadAssets();
		m_MusicDatabase.UnloadAssets();
	}
	void AudioManager::Update()
	{
		_PurgedFinishedSounds();
	}
	void AudioManager::_PurgedFinishedSounds()
	{
		std::vector<sf::Sound*>::iterator it = m_PlayingSounds.begin();

		while (it != m_PlayingSounds.end())
		{
			if ((*it)->getStatus() == sf::Sound::Status::Stopped)
			{
				delete* it;
				it = m_PlayingSounds.erase(it); // erase retourne le prochain itérateur valide
			}
			else
			{
				++it;
			}
		}
	}
}