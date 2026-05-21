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
	{
		auto& n = EventManager::Instance();
		n.Register<PlayMusicEvent>(this);
		n.Register<PlaySoundEvent>(this);
		n.Register<PauseEvent>(this);
		n.Register<LoadingStart>(this);
		n.Register<LoadingComplete>(this);
	}
	AudioManager::~AudioManager()
	{
		UnloadAll();
	}
	void AudioManager::OnEvent(const PlaySoundEvent& event)
	{
		PlaySound(event.Path, event.volume);
	}
	void AudioManager::OnEvent(const PlayMusicEvent& event)
	{
		PlayMusic(event.Path, event.loop, event.volume);
	}
	void AudioManager::OnEvent(const PauseEvent& event)
	{
		if (event.pause) {
			PauseAllSounds();
		}
		else {
			ResumeAllSounds();
		}
	}
	void AudioManager::OnEvent(const LoadingStart& event)
	{
		UnloadAll();
	}
	void AudioManager::OnEvent(const LoadingComplete& event)
	{
		if (event.m_GameMode == GameMode::InGame && event.m_MapData != nullptr && !event.m_MapData->BgAudioPath.empty())
		{
			PlayMusic(event.m_MapData->BgAudioPath, true, 65.f);
		}
	}
	void AudioManager::PlaySound(const std::string& path, float volume)
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
	void AudioManager::PlayMusic(const std::string& path, bool loop, float volume)
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
	void AudioManager::PauseMusic(const std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Stopped || music->getStatus() == sf::Music::Status::Paused) return;
		music->pause();
	}
	void AudioManager::ResumeMusic(const std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Playing) return;
		music->play();
	}
	void AudioManager::StopMusic(const std::string& path)
	{
		sf::Music* music = m_MusicDatabase.GetAsset(path);
		if (music == nullptr || music->getStatus() == sf::Music::Status::Stopped) return;
		music->stop();
	}
	void AudioManager::PauseAllSounds()
	{
		for (auto& s : m_PlayingSounds) 
		{
			if (s != nullptr && s->getStatus() == sf::Sound::Status::Playing)
			{
				s->pause();
			}
		}
	}
	void AudioManager::ResumeAllSounds()
	{
		for (auto& s : m_PlayingSounds)
		{
			if (s != nullptr && s->getStatus() == sf::Sound::Status::Paused)
			{
				s->play();
			}
		}
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
