#pragma once
#include <Game/AudioManager.hpp>

namespace Game
{
	AudioManager::AudioManager()
		: m_SoundDatabase(SoundDatabase::Instance())
		, m_MusicDatabase(MusicDatabase::Instance())
		, m_MasterVolume(1.0f)
		, m_SoundVolume(1.0f)
		, m_MusicVolume(1.0f)
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
		sf::SoundBuffer* buffer = m_SoundDatabase.Load(path);
		if (buffer == nullptr)
			return;

		sf::Sound* sound = new sf::Sound(*buffer);

		// Volume final = volume local * volume sons * volume master (ramenés à 0-1)
		float finalVolume = volume * m_SoundVolume * m_MasterVolume;
		sound->setVolume(finalVolume);

		m_PlayingSounds.push_back(sound);
		sound->play();
	}
	void AudioManager::PlayMusic(std::string& path, bool loop, float volume)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		float finalVolume = volume * m_MusicVolume * m_MasterVolume;
		if (music != nullptr)
		{
			if (music->getStatus() != sf::Music::Status::Stopped) return;
			music->setLooping(loop);
			music->setVolume(finalVolume);
			music->play();
			return;
		}
		music = m_MusicDatabase.Load(path);
		music->setLooping(loop);
		music->setVolume(finalVolume);
		music->play();
	}
	void AudioManager::PauseMusic(std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Stopped || music->getStatus() == sf::Music::Status::Paused) return;
		music->pause();
	}
	void AudioManager::ResumeMusic(std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Playing) return;
		music->play();
	}
	void AudioManager::StopMusic(std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Stopped) return;
		music->stop();
	}
	void AudioManager::SetMasterVolume(float volume)
	{
		m_MasterVolume = volume / 100.f;
		_UpdateVolume();
	}
	void AudioManager::SetSoundVolume(float volume)
	{
		m_SoundVolume = volume / 100.f;
		_UpdateVolume();
	}
	void AudioManager::SetMusicVolume(float volume)
	{
		m_MusicVolume = volume / 100.f;
		_UpdateVolume();
	}
	void AudioManager::StopAll()
	{
		for (auto& m : m_MusicDatabase.GetAssets())
		{
			if (m == nullptr || m->getStatus() == sf::Music::Status::Stopped) continue;
			m->stop();
		}
		for (auto& s : m_PlayingSounds)
		{
			if (s == nullptr || s->getStatus() == sf::Sound::Status::Stopped) continue;
			s->stop();
		}
	}
	void AudioManager::UnloadAll()
	{
		StopAll();
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
	void AudioManager::_UpdateVolume()
	{
		for (auto& s : m_PlayingSounds)
		{
			if (s != nullptr) {
				float previousVol = s->getVolume();
				s->setVolume(previousVol * m_SoundVolume * m_MasterVolume);
			}
		}
		for (auto& m : m_MusicDatabase.GetAssets())
		{
			if (m != nullptr) {
				float previousVol = m->getVolume();
				m->setVolume(previousVol * m_MusicVolume * m_MasterVolume);
			}
		}
	}
}