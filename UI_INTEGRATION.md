# UI Integration Guide

This document explains how to wire up the UI layer that was just generated.
Nothing in `src/Game/GameManager.cpp` or in the engine has been modified —
all new screens and widgets are self-contained behind setters and callbacks.

---

## 1. What was added

### New widgets (`include/Game/UI/Widgets/`)
| Widget         | Purpose                                                         |
| -------------- | --------------------------------------------------------------- |
| `UIImage`      | Static texture display (logos, icons, biome previews).          |
| `UIPanel`      | Decorative background rectangle (textured or solid color).      |
| `UIToggle`     | On/off checkbox-style widget.                                   |
| `UISlider`     | Horizontal value slider with a draggable handle (e.g. volume).  |
| `UIMusicPlayer`| Composite: panel + title + prev/play/next/stop + progress bar.  |

All seven widgets are included in `Widgets.hpp`, so any screen that does
`#include <Game/UI.hpp>` gets them for free.

### New screens (`include/Game/UI/Menus/`)
| Screen              | Where it appears                                        |
| ------------------- | ------------------------------------------------------- |
| `TitleScreen`       | Boot of the game.                                       |
| `LevelSelectScreen` | After clicking *Play* on the title.                     |
| `PauseMenu`         | Overlay during gameplay when paused.                    |
| `SettingsScreen`    | Reachable from Title and from Pause.                    |
| `CreditsScreen`     | Reachable from Title.                                   |
| `VictoryScreen`     | When the player completes a level.                      |
| `GameOverScreen`    | When the player fails a level.                          |

### Modified files
* `include/Game/UI/Menus/InGameUI.h` — added `m_MusicPlayer` + `GetMusicPlayer()`.
* `src/Game/UI/Menu/InGameUI.cpp`   — instantiates the music player.
* `include/Game/UI/Widgets.hpp`     — now also includes the four new widgets.

### New assets
* `assets/UI/` reorganized into typed sub-folders (Buttons, Sliders, Checkboxes,
  Panels, MusicControls, LevelSelect, Bars, Toasts, Tooltips, Icons, Scroll,
  Tabs, TextField, Statistics). All sourced from `assets/Textures/.IDK/gui/`
  except for `play_*.png`, `pause_*.png`, `stop_*.png` which were generated.
* `assets/Settings/settings.json` — defaults for `GameSettings`.

---

## 2. How to display a screen

The existing `UIManager::SetRoot<T>()` is the entry point. Each screen exposes
setters for its callbacks. The general recipe is:

```cpp
#include <Game/UI/Menus/TitleScreen.hpp>

void GameManager::ShowTitle()
{
    auto& title = UIManager::Instance().SetRoot<TitleScreen>();
    title.SetOnPlay    ([this] { ShowLevelSelect(); });
    title.SetOnSettings([this] { ShowSettings(/*returnTo=*/Screen::Title); });
    title.SetOnCredits ([this] { ShowCredits(); });
    title.SetOnQuit    ([this] { RequestQuit(); });
}
```

Each `ShowX()` should also drive `m_CurrentMode` / `m_CurrentState` and emit a
`LoadingStart` / `LoadingComplete` pair if you want `AudioManager` to switch
playlists automatically (which it already does for `GameMode::MainMenu` and
`GameMode::InGame`).

---

## 3. Screen-by-screen wiring

### 3.1 TitleScreen

```cpp
auto& t = UIManager::Instance().SetRoot<TitleScreen>();
t.SetOnPlay    ([this] { ShowLevelSelect(); });
t.SetOnSettings([this] { ShowSettings(/*returnTo=*/Screen::Title); });
t.SetOnCredits ([this] { ShowCredits(); });
t.SetOnQuit    ([this] { /* break the main loop, save settings, exit */ });
```

Audio: emit `LoadingComplete` with `m_GameMode = GameMode::MainMenu` so
`AudioManager::OnEvent(LoadingComplete)` starts the `Musics\0.Menus` playlist.

### 3.2 LevelSelectScreen

```cpp
auto& ls = UIManager::Instance().SetRoot<LevelSelectScreen>();
ls.SetLevels(/* optional — defaults include the 7 ship maps */);
ls.SetOnPlayLevel([this](const std::string& mapPath) { LoadLevel(mapPath); });
ls.SetOnBack     ([this] { ShowTitle(); });
```

`LoadLevel(mapPath)` should:
1. Emit `LoadingStart{Purpose::LoadGame, GameMode::InGame}`.
2. Load the `MapData` via `MapDatabase::Instance().Load(mapPath)`.
3. Build the world.
4. Emit `LoadingComplete{Purpose::LoadGame, GameMode::InGame, mapData}` — this
   triggers the playlist for that map's biome via `MapData::BgPlaylist`.

### 3.3 InGameUI (HUD) + Music player

`InGameUI` already exists and is unchanged behaviour-wise. The new
`UIMusicPlayer` lives in the top-right corner. **You must wire its callbacks
once after constructing the HUD:**

```cpp
auto& hud = UIManager::Instance().SetRoot<InGameUI>();
auto* mp  = hud.GetMusicPlayer();

mp->SetOnPlayPause([] {
    // Toggle the *playlist* play/pause via AudioManager.
    // The cleanest path is to expose AudioManager::TogglePlaylistPause().
    // For now you can call PauseMusic/ResumeMusic on the current track:
    auto& am = AudioManager::Instance();
    // TODO: AudioManager doesn't yet expose IsPlaylistPlaying() —
    // add a helper there and pause/resume the m_PlaylistCurrent track.
});

mp->SetOnNext([] {
    // Advance to the next track. Expose AudioManager::_AdvancePlaylist()
    // publicly (rename to AdvancePlaylist()) and call it here:
    // AudioManager::Instance().AdvancePlaylist();
});

mp->SetOnPrev([] {
    // The current AudioManager doesn't support previous (the playlist index
    // only goes forward). Easiest fix: track the last-played index and add
    // AudioManager::RewindPlaylist() that decrements m_PlaylistIndex and
    // restarts the track at that index.
});

mp->SetOnStop([] {
    AudioManager::Instance().StopPlaylist();
});
```

To keep the music player's title/progress bar in sync, call from your per-frame
update loop:

```cpp
void GameManager::UpdateMusicPlayerWidget(UIMusicPlayer& mp)
{
    auto& am = AudioManager::Instance();
    // TODO: expose getters in AudioManager:
    //   const std::string& GetCurrentTrackName() const;
    //   bool  IsPlaylistPlaying() const;
    //   float GetCurrentTrackPosition01() const;   // 0..1
    //   sf::Time GetCurrentTrackTime() const;
    //   sf::Time GetCurrentTrackDuration() const;

    mp.SetTrackTitle(am.GetCurrentTrackName());
    mp.SetPlaying  (am.IsPlaylistPlaying());
    mp.SetProgress (am.GetCurrentTrackPosition01());

    // Optional: format mm:ss
    auto fmt = [](sf::Time t) {
        int s = int(t.asSeconds());
        return std::to_string(s / 60) + ":" + (s%60<10 ? "0":"") + std::to_string(s%60);
    };
    mp.SetTimeLabels(fmt(am.GetCurrentTrackTime()), fmt(am.GetCurrentTrackDuration()));
}
```

### 3.4 PauseMenu

```cpp
auto& p = UIManager::Instance().SetRoot<PauseMenu>();
p.SetOnResume   ([this] { ShowInGame(); World::Instance().Pause(false); });
p.SetOnSettings ([this] { ShowSettings(/*returnTo=*/Screen::Pause); });
p.SetOnMainMenu ([this] { TeardownLevel(); ShowTitle(); });
p.SetOnQuit     ([this] { /* graceful shutdown */ });

// The embedded music player needs the same wiring as in the HUD:
WireMusicPlayer(*p.GetMusicPlayer());
```

Tip: extract `WireMusicPlayer(UIMusicPlayer&)` into a helper so you call it
identically from `InGameUI` and `PauseMenu`.

### 3.5 SettingsScreen

```cpp
auto& s = UIManager::Instance().SetRoot<SettingsScreen>();
s.SetCurrent(m_Settings);   // current GameSettings loaded from disk

s.SetOnApply([this](const GameSettings& ns) {
    m_Settings = ns;

    // Push to AudioManager. Volumes are 0..100 as defined in GameSettings.
    auto& am = AudioManager::Instance();
    am.SetMasterVolume(ns.MasterVolume);
    am.SetSoundVolume (ns.SoundVolume);
    am.SetMusicVolume (ns.MusicVolume);

    // Push to window (TODO — implement on RenderingManager)
    // RenderingManager::Instance().SetFullscreen(ns.Fullscreen);
    // RenderingManager::Instance().SetVSync     (ns.VSync);

    SaveSettings(m_Settings, "Settings/settings.json");
    ShowPreviousScreen();  // pop back to Title or Pause depending on context
});

s.SetOnBack([this] { ShowPreviousScreen(); });
```

`SaveSettings` is a one-liner via glaze:

```cpp
// MapDatabase shows the equivalent loading pattern.
glz::write_file_json(settings, fullPath, std::string{});
```

### 3.6 CreditsScreen

```cpp
auto& c = UIManager::Instance().SetRoot<CreditsScreen>();
c.SetOnBack([this] { ShowTitle(); });
```

The content of the credits is edited in
`src/Game/UI/Menu/CreditsScreen.cpp` (look for `CREDITS_LINES`).

### 3.7 VictoryScreen / GameOverScreen

```cpp
auto& v = UIManager::Instance().SetRoot<VictoryScreen>();
v.SetStats({
    { "Score", std::to_string(score) },
    { "Time",  FormatTime(elapsed)   },
    { "Rank",  rank }
});
v.SetOnReplay   ([this] { ReloadCurrentLevel(); });
v.SetOnNextLevel([this] { LoadNextLevel(); });
v.SetOnMainMenu ([this] { ShowTitle(); });
```

```cpp
auto& over = UIManager::Instance().SetRoot<GameOverScreen>();
over.SetReason("You ran out of money");   // optional, can be ""
over.SetOnRetry   ([this] { ReloadCurrentLevel(); });
over.SetOnMainMenu([this] { ShowTitle(); });
```

---

## 4. Settings persistence

The default `settings.json` is at `assets/Settings/settings.json`. The struct
`Game::GameSettings` (in `include/Game/Settings/GameSettings.hpp`) maps 1:1
to the JSON keys, so glaze handles the (de)serialization for free.

Suggested wrapper (write yourself in `GameManager` or a new
`SettingsManager`):

```cpp
#include <glaze/glaze.hpp>

bool LoadSettings(GameSettings& out, const std::string& relativePath)
{
    namespace fs = std::filesystem;
    std::string full = (fs::current_path() / "assets" / relativePath).string();
    auto err = glz::read_file_json(out, full, std::string{});
    return !err;
}

bool SaveSettings(const GameSettings& in, const std::string& relativePath)
{
    namespace fs = std::filesystem;
    std::string full = (fs::current_path() / "assets" / relativePath).string();
    auto err = glz::write_file_json(in, full, std::string{});
    return !err;
}
```

Call `LoadSettings` in `GameManager::Start()` and apply the values to
`AudioManager` / `RenderingManager` right after.

---

## 5. AudioManager extensions needed

The current `AudioManager` (`include/Game/AudioManager.hpp`,
`src/Game/AudioManager.cpp`) already supports playlists end-to-end (shuffle +
auto-next). To fully wire `UIMusicPlayer`, you'll want to **add three small
helpers** — none of them require changing existing behavior:

```cpp
// in include/Game/AudioManager.hpp, public:
const std::string& GetCurrentTrackPath() const;     // returns m_PlaylistTracks[m_PlaylistIndex] or "".
bool   IsPlaylistPlaying() const;                   // m_PlaylistCurrent && Status == Playing
sf::Time GetCurrentTrackTime() const;               // m_PlaylistCurrent ? m_PlaylistCurrent->getPlayingOffset() : Time::Zero
sf::Time GetCurrentTrackDuration() const;           // m_PlaylistCurrent ? m_PlaylistCurrent->getDuration() : Time::Zero
float GetCurrentTrackPosition01() const;            // time / duration, clamped

void TogglePlaylistPause();                         // pause/resume m_PlaylistCurrent
void AdvancePlaylist();                             // == current _AdvancePlaylist(), make public
void RewindPlaylist();                              // -- m_PlaylistIndex, replay
```

Each is a 2-5 line method — see the existing `StopPlaylist()` for the pattern.

---

## 6. Screen transitions cheat sheet

```
              ┌──────────────┐
              │  TitleScreen │
              └──────┬───────┘
        Play         │ Settings    Credits        Quit
        ▼            ▼             ▼              ▼
┌──────────────┐ ┌──────────┐ ┌─────────┐  (exit application)
│ LevelSelect  │ │ Settings │ │ Credits │
└──────┬───────┘ └────┬─────┘ └────┬────┘
       │ pick a map  │ back       │ back
       ▼             ▼            ▼
┌──────────────┐  back to Title (or Pause if reached from Pause)
│   InGameUI   │◄────────────────────────────────────┐
└──────┬───────┘                                     │
       │ ESC / pause btn          Win                │ Lose
       ▼                           ▼                  ▼
┌──────────────┐  Resume     ┌──────────┐       ┌────────────┐
│  PauseMenu   │────────────►│ Victory  │       │ GameOver   │
└──────┬───────┘             └────┬─────┘       └─────┬──────┘
       │ Main Menu / Quit         │ Replay/Next/Menu  │ Retry/Menu
       └──────────────────────────┴───────────────────┘
```

---

## 7. Files to add to your `.vcxproj`

All new files are already added to `MineRail Valley.vcxproj` (look for the
ClCompile / ClInclude entries grouped around the existing UI files). If
something fails to compile, double-check that the file is referenced.
