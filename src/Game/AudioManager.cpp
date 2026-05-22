#pragma once
#include <Game/AudioManager.hpp>
#include<Game/Utils/Time.hpp>

#include <algorithm>
#include <filesystem>
#include <random>

namespace Game
{
	namespace
	{
		// File extensions we treat as audio tracks for playlist scans.
		bool _IsAudioFile(const std::filesystem::path& p)
		{
			if (!p.has_extension()) return false;
			std::string ext = p.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(),
				[](unsigned char c) { return std::tolower(c); });
			return ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac";
		}
	}
	AudioManager::AudioManager()
		: m_SoundDatabase(SoundDatabase::Instance())
		, m_MusicDatabase(MusicDatabase::Instance())
		, m_MasterVolume(1.0f)
		, m_SoundVolume(1.0f)
		, m_MusicVolume(1.0f)
		, m_PlaylistIndex(0)
		, m_PlaylistVolume(0.f)
		, m_MusicPlayer(nullptr)
	{
		m_PlaylistCurrent = { nullptr, 0.f };
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
		_ResetPlaylist();
		UnloadAll();
	}
	void AudioManager::OnEvent(const LoadingComplete& event)
	{
		// Main menu: play the dedicated menu playlist.
		if (event.m_GameMode == GameMode::MainMenu)
		{
			PlayPlaylist((std::filesystem::path("Musics") / "0.Menus").string(), 60.f);
			return;
		}

		// In-game: prefer the playlist field on the MapData; fall back to the legacy single
		// BgAudioPath for backward compatibility with old maps.
		if (event.m_GameMode == GameMode::InGame && event.m_MapData != nullptr)
		{
			const MapData& md = *event.m_MapData;
			if (!md.BgPlaylist.empty())
			{
				PlayPlaylist(md.BgPlaylist, 65.f);
			}
			else if (!md.BgAudioPath.empty())
			{
				PlayMusic(md.BgAudioPath, true, 65.f);
			}
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
		if (music == nullptr)
		{
			music = m_MusicDatabase.Load(path);

			if (music == nullptr) return;
		}

		float finalVolume = volume * m_MusicVolume * m_MasterVolume;

		music->setLooping(loop);
		music->setVolume(finalVolume);
		music->play();

		m_PlaylistCurrent = { music, finalVolume };
	}
	void AudioManager::PlayPlaylist(const std::string& folder, float volume)
	{
		if (folder.empty()) return;

		// Stop and clear any previous playlist so PlayMusic below doesn't fight with an old track.
		StopPlaylist();

		// Resolve the playlist folder against the asset root used by MusicDatabase.
		// MusicDatabase::Load already prepends DatabasePath (the assets dir), so for the FS scan
		// we need to do the same here.
		std::filesystem::path baseDir = std::filesystem::path(m_MusicDatabase.DatabasePath) / folder;
		std::error_code ec;
		if (!std::filesystem::is_directory(baseDir, ec))
		{
			// No directory: nothing to play. Keep playlist inactive.
			m_PlaylistFolder.clear();
			m_PlaylistTracks.clear();
			return;
		}

		// Collect all audio files in the folder. We store paths *relative to assets/* so that
		// MusicDatabase::Load_Impl resolves them correctly via its DatabasePath prefix.
		m_PlaylistTracks.clear();
		for (auto const& entry : std::filesystem::directory_iterator(baseDir, ec))
		{
			if (ec) break;
			if (!entry.is_regular_file()) continue;
			if (!_IsAudioFile(entry.path())) continue;

			std::filesystem::path rel = std::filesystem::path(folder) / entry.path().filename();
			m_PlaylistTracks.push_back(rel.string());
		}

		if (m_PlaylistTracks.empty())
		{
			m_PlaylistFolder.clear();
			return;
		}

		// Shuffle for variety. New std::mt19937 seeded by random_device per call.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::shuffle(m_PlaylistTracks.begin(), m_PlaylistTracks.end(), gen);

		m_PlaylistFolder = folder;
		m_PlaylistIndex = 0;
		m_PlaylistVolume = volume;

		// Start the first track. Looping is OFF on every playlist track so that Update() can detect
		// the natural end-of-track and move on.
		const std::string& firstPath = m_PlaylistTracks[m_PlaylistIndex];
		PlayMusic(firstPath, /*loop*/ false, m_PlaylistVolume);
		m_PlaylistCurrent.music = m_MusicDatabase.GetAsset(firstPath);
	}
	void AudioManager::StopPlaylist()
	{
		if (m_PlaylistCurrent.music != nullptr)
		{
			if (m_PlaylistCurrent.music->getStatus() != sf::Music::Status::Stopped)
			{
				m_PlaylistCurrent.music->stop();
			}
		}
		_ResetPlaylist();
	}
	void AudioManager::TogglePlaylistPause()
	{
		if (m_PlaylistCurrent.music == nullptr) return;
		const auto st = m_PlaylistCurrent.music->getStatus();
		if (st == sf::Music::Status::Playing)
			m_PlaylistCurrent.music->pause();
		else if (st == sf::Music::Status::Paused)
			m_PlaylistCurrent.music->play();
	}
	void AudioManager::AdvancePlaylist()
	{
		// Public alias of the internal advance — auto-next is what the "Next"
		// button on the UIMusicPlayer needs.
		_AdvancePlaylist();
	}
	void AudioManager::RewindPlaylist()
	{
		if (m_PlaylistTracks.empty()) return;

		// Stop the currently-playing track via the pointer (more reliable than
		// going through StopMusic(path) which has to re-resolve via the asset
		// database and would silently no-op if the path mapping was stale).
		if (m_PlaylistCurrent.music != nullptr
			&& m_PlaylistCurrent.music->getStatus() != sf::Music::Status::Stopped)
		{
			m_PlaylistCurrent.music->stop();
		}

		if (m_PlaylistIndex == 0) m_PlaylistIndex = m_PlaylistTracks.size() - 1;
		else                       m_PlaylistIndex--;
		const std::string& nextPath = m_PlaylistTracks[m_PlaylistIndex];
		PlayMusic(nextPath, /*loop*/ false, m_PlaylistVolume);
		m_PlaylistCurrent.music = m_MusicDatabase.GetAsset(nextPath);
	}
	// ─── Introspection ────────────────────────────────────────────────────────
	bool AudioManager::IsPlaylistPlaying() const
	{
		return m_PlaylistCurrent.music != nullptr
			&& m_PlaylistCurrent.music->getStatus() == sf::Music::Status::Playing;
	}
	bool AudioManager::IsPlaylistPaused() const
	{
		return m_PlaylistCurrent.music != nullptr
			&& m_PlaylistCurrent.music->getStatus() == sf::Music::Status::Paused;
	}
	std::string AudioManager::GetCurrentTrackPath() const
	{
		if (m_PlaylistTracks.empty()) return {};
		if (m_PlaylistIndex >= m_PlaylistTracks.size()) return {};
		return m_PlaylistTracks[m_PlaylistIndex];
	}
	std::string AudioManager::GetCurrentTrackName() const
	{
		std::string p = GetCurrentTrackPath();
		if (p.empty()) return "—";
		// Take last segment after \ or /, drop extension. Stays portable enough
		// without dragging std::filesystem into the header.
		std::size_t slash = p.find_last_of("\\/");
		std::string base = (slash == std::string::npos) ? p : p.substr(slash + 1);
		std::size_t dot = base.find_last_of('.');
		if (dot != std::string::npos) base = base.substr(0, dot);
		return base;
	}
	sf::Time AudioManager::GetCurrentTrackTime() const
	{
		return m_PlaylistCurrent.music ? m_PlaylistCurrent.music->getPlayingOffset() : sf::Time::Zero;
	}
	sf::Time AudioManager::GetCurrentTrackDuration() const
	{
		return m_PlaylistCurrent.music ? m_PlaylistCurrent.music->getDuration() : sf::Time::Zero;
	}
	float AudioManager::GetCurrentTrackPosition01() const
	{
		if (m_PlaylistCurrent.music == nullptr) return 0.f;
		const sf::Time t = m_PlaylistCurrent.music->getPlayingOffset();
		const sf::Time d = m_PlaylistCurrent.music->getDuration();
		if (d.asMicroseconds() <= 0) return 0.f;
		float r = t.asSeconds() / d.asSeconds();
		return std::clamp(r, 0.f, 1.f);;
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
	float AudioManager::GetMasterVolume() const
	{
		return m_MasterVolume;
	}
	float AudioManager::GetSoundVolume() const
	{
		return m_SoundVolume;
	}
	float AudioManager::GetMusicVolume() const
	{
		return m_MusicVolume;
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

		// Playlist auto-advance: when the current track has stopped naturally
		// (i.e. end-of-file), move on to the next one. This must run
		// *unconditionally* (not gated by the UI widget) — without it the
		// playlist would just stop after the first track.
		if (!m_PlaylistFolder.empty()
			&& m_PlaylistCurrent.music != nullptr
			&& m_PlaylistCurrent.music->getStatus() == sf::Music::Status::Stopped)
		{
			_AdvancePlaylist();
		}

		// UI refresh at 1 Hz — title/time/progress only need second-level
		// resolution, no need to spam ReDraw at 60 fps.
		if (!m_MusicPlayer) return;
		m_UiUpdateTimer += Time::GetDeltaTime().asMilliseconds();
		if (m_UiUpdateTimer >= 1000.f)
		{
			m_UiUpdateTimer -= 1000.f;
			m_MusicPlayer->UpdatePlayer();
		}
	}
	void AudioManager::RegisterMusicPlayer(UIMusicPlayer* player)
	{
		m_MusicPlayer = player;
	}
	void AudioManager::UnRegisterMusicPlayer()
	{
		m_MusicPlayer = nullptr;
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

		// Sounds
		for (auto& s : m_PlayingSounds)
		{
			if (s != nullptr)
			{
				s->setVolume(100.f * m_SoundVolume * m_MasterVolume);
			}
		}

		// Musique actuelle
		if (m_PlaylistCurrent.music != nullptr)
		{
			float finalVol =
				m_PlaylistCurrent.baseVolume *
				m_MusicVolume *
				m_MasterVolume;

			m_PlaylistCurrent.music->setVolume(finalVol);
		}

	}
	void AudioManager::_AdvancePlaylist()
	{
		if (m_PlaylistTracks.empty())
		{
			_ResetPlaylist();
			return;
		}

		// CRITICAL: stop the previously playing track before starting the next
		// one. Without this, when the user pressed "Next" while the current
		// track was still playing, the two tracks would overlap (the user-
		// reported "musiques s'additionnent" bug). It's a no-op when called
		// from the auto-advance path because the track is already Stopped.
		if (m_PlaylistCurrent.music != nullptr
			&& m_PlaylistCurrent.music->getStatus() != sf::Music::Status::Stopped)
		{
			m_PlaylistCurrent.music->stop();
		}

		m_PlaylistIndex++;
		if (m_PlaylistIndex >= m_PlaylistTracks.size())
		{
			// End of playlist: re-shuffle and loop back so consecutive listening still feels fresh.
			std::random_device rd;
			std::mt19937 gen(rd());
			std::shuffle(m_PlaylistTracks.begin(), m_PlaylistTracks.end(), gen);
			m_PlaylistIndex = 0;
		}

		const std::string& nextPath = m_PlaylistTracks[m_PlaylistIndex];
		PlayMusic(nextPath, /*loop*/ false, m_PlaylistVolume);
		m_PlaylistCurrent.music = m_MusicDatabase.GetAsset(nextPath);
	}
	void AudioManager::_ResetPlaylist()
	{
		m_PlaylistFolder.clear();
		m_PlaylistTracks.clear();
		m_PlaylistIndex = 0;
		m_PlaylistVolume = 0.f;
		m_PlaylistCurrent = { nullptr, 0.f };
	}
}
