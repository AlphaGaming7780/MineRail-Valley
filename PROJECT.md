# MineRail Valley — Project Documentation

A 2D top-down train / rail building game written in C++23 with SFML 3.
Players lay tracks across procedurally-styled biomes to connect stations and
keep the trains running. This document is the authoritative map of the
codebase: it explains every module, lists the conventions, and tracks what is
done versus what is still pending.

---

## 1. At a glance

| Metric                          |        Value |
| ------------------------------- | ------------:|
| Lines of project C++ code       |       10 035 |
| Source files (`.cpp`+`.h`/`.hpp`)|     ~115 |
| Modules (top-level directories) |            8 |
| Maps                            |            7 |
| Tile types                      |           44 |
| Tile textures                   |           46 |
| UI sprites                      |           77 |
| Music tracks                    |           44 (6 playlists) |
| SFX                             |            5 (more to come) |
| Tile placements across all maps |       39 680 |

Built with **Visual Studio 2022 + MSVC v143 / v145** (C++23, `stdcpp23` in the
vcxproj). Targets x64 Debug & Release. SFML, glaze, magic_enum, mbedtls and
libssh2 are vendored into `include/` and `lib/`.

---

## 2. Repository layout

```
MineRail-Valley/
├── MineRail Valley.sln       ← Visual Studio solution
├── MineRail Valley.vcxproj   ← Project file
├── main.cpp                  ← Entry point: GameManager::Instance().Start()
├── UI_INTEGRATION.md         ← Quick guide for wiring screens
├── PROJECT.md                ← (this file)
│
├── include/                  ← All headers
│   ├── Game/                 ← The game code
│   │   ├── AssetDatabase/    ← Templated asset cache + map/tile/music/...
│   │   ├── AudioManager.hpp  ← SFML audio singleton, playlist transport
│   │   ├── Events/           ← Event manager + observer + event structs
│   │   ├── GameManager.hpp   ← Orchestrator (main loop + screen navigation)
│   │   ├── GameObjects/      ← Track/Train/Wagon/Station entity classes
│   │   ├── GameStates.h      ← enums GameState / GameMode / Purpose
│   │   ├── InputsActionSets/ ← Mapping camera/tool inputs
│   │   ├── Rendering/        ← Camera, RenderingManager, GameWindow
│   │   ├── Settings/         ← GameSettings + SettingsManager (NEW)
│   │   ├── Systems/          ← ECS-ish: Camera/Track/Bulldoze/Train/Default
│   │   ├── UI/               ← UIManager, UIElement, UIGroup, UIWidget, …
│   │   │   ├── Menus/        ← Title, LevelSelect, Pause, Settings, Credits,
│   │   │   │                   Victory, GameOver, InGameUI, LoadingScreen
│   │   │   └── Widgets/      ← Button, Label, ProgressBar, Image, Panel,
│   │   │                       Toggle, Slider, MusicPlayer
│   │   ├── Utils/            ← Time, math, raycast, terrain helpers
│   │   └── World.h           ← Singleton: world entities + systems
│   ├── PallasEngine/         ← In-house logging
│   ├── SFML/                 ← Vendored SFML 3
│   ├── glaze/                ← Vendored JSON serializer
│   ├── magic_enum/           ← Enum reflection
│   ├── mbedtls/, psa/        ← Crypto (for libssh2)
│   └── libssh2*.h            ← SFTP support (unused for now)
│
├── src/                      ← Implementations mirroring include/
│
├── lib/                      ← .lib files (sfml, mbedtls, ssh2, …)
│
├── assets/                   ← Runtime assets copied by the post-build event
│   ├── Fonts/                ← TTF
│   ├── Maps/*.json           ← 7 levels (see §6)
│   ├── Tiles/*.json          ← 44 tile definitions
│   ├── Textures/*.png        ← 46 top-down block textures
│   │   └── .IDK/             ← raw Minecraft texture pack (source, not shipped)
│   ├── UI/                   ← 77 UI sprites in 14 categories (see §4.4)
│   ├── Musics/               ← 6 playlists: 0.Menus, 1.Night_Drive,
│   │                            2.Industrial_Ambiant, 3.Lo-Fi_Journey,
│   │                            4.Zen, 5.Neons_n_Rain
│   ├── Sounds/               ← 5 SFX (more needed, §9)
│   ├── Stations/, Tracks/, Trains/   ← gameplay-object data
│   └── Settings/settings.json    ← user-tunable settings (NEW)
│
├── OLD/                      ← Historical files, no longer compiled
└── bin/, build/              ← MSVC outputs
```

---

## 3. Module by module

### 3.1 `Game::GameManager` — orchestrator

- Singleton, lives for the whole app lifetime.
- `Start()` opens the SFML window, initializes log + event manager, runs `MainLoop()`, then unloads everything cleanly.
- `MainLoop()` is the canonical "tick every frame" loop. Each iteration calls, in order: `renderManager.NewFrame()`, `inputManager.ResetEdges()`, `eventManager.process()`, `inputManager.Update()`, `audioManager.Update()`, `OnUpdate()`, `world.Update()`, `uiManager.Update()`, `renderManager.Render()`, `uiManager.Draw()`, `gameWindow.Display()`, `Time::Update()`.
- `OnStart()` (NEW) now: loads settings, applies them, builds the world systems, builds the level order list, and calls `ShowTitle()`.
- `Show*()` methods centralise screen navigation (see §4.6).
- `Load(GameMode, Purpose, MapData*)` is the canonical "switch context" call: it shows `UILoadingScreen`, fires the `LoadingStart` + `LoadingComplete` observers, then returns. `AudioManager` listens on those events to swap playlists.
- `LoadLevel / ReloadCurrentLevel / LoadNextLevel / TeardownLevel` handle level lifecycle.

### 3.2 `Game::World`

- Holds the singleton ECS-lite registry. Owns the set of `System`s and game objects.
- `Pause(bool)`, `IsPaused()` for the freeze used by the Pause menu.
- `Shutdown()` releases everything on app exit.

### 3.3 `Game::Systems::*`

Each system inherits `SystemBase` and overrides `OnGameLoadingStart / OnGameLoadingComplete / Update`.

- **CameraSystem** — pan/zoom + camera shake.
- **DefaultToolSystem** — neutral pointer mode (just inspection).
- **TrackToolSystem** — places/edits track tiles.
- **BulldozeToolSystem** — removes track tiles.
- **TrainSystem** — moves and ticks trains.

The HUD lets the player switch tools via the three buttons at the bottom.

### 3.4 `Game::AudioManager`

A singleton SFML audio façade. Subscribes to `PlaySoundEvent`,
`PlayMusicEvent`, `PauseEvent`, `LoadingStart`, `LoadingComplete`. Major
responsibilities:

- **PlaySound / PlayMusic** — single-shot playback through the asset databases.
- **Playlists** — `PlayPlaylist(folder, volume)` scans a folder, shuffles, plays one track at a time, auto-advances when each track ends (detected in `Update()`), reshuffles at the end.
- **Transport** (NEW) — `TogglePlaylistPause / AdvancePlaylist / RewindPlaylist / StopPlaylist`.
- **Introspection** (NEW) — `IsPlaylistPlaying / GetCurrentTrackName / GetCurrentTrackTime / GetCurrentTrackDuration / GetCurrentTrackPosition01`. Used by `UIMusicPlayer` and `GameManager::UpdateMusicPlayerWidget()` to drive the widget.
- **Event-driven mode switching** — `OnEvent(LoadingComplete)` picks the right playlist based on `GameMode`: `MainMenu` → `Musics\0.Menus`, `InGame` → `MapData::BgPlaylist`, fallback to `BgAudioPath` for legacy single-track maps.

### 3.5 `Game::Events::EventManager`

Pub/sub manager templated by event type. `Register<T>(observer)` adds an observer; `GetObservers<T>()` returns the current list; `process()` drains the SFML event queue.

`LoadingStart` and `LoadingComplete` carry the `Purpose`, `GameMode`, and a `MapData*` so subsystems know what's happening.

### 3.6 `Game::UI::*` — the UI layer

Hierarchical retained-mode UI. The `UIManager` holds a single root (`UIGroup`), draws into an internal `sf::RenderTexture` and re-draws only the invalidated subtrees.

Anchor system (`UIAnchor::TopLeft/TopRight/BottomLeft/BottomRight/Center`) makes elements reposition correctly on window resize.

Widgets (`include/Game/UI/Widgets/`):

| Widget         | Role                                                   |
| -------------- | ------------------------------------------------------ |
| `UIButton`     | Triple-state (normal/hover/clicked) clickable button. NEW: plays the "Menu Selection Click" SFX on every OnClick. |
| `UILabel`      | SFML text rendering, color/size/text setters.          |
| `UIProgressBar`| Frame + filled bar + end cap.                          |
| `UIImage`      | (NEW) Static texture display.                          |
| `UIPanel`      | (NEW) Background rectangle (textured or color), with optional mouse blocking. |
| `UIToggle`     | (NEW) On/off checkbox.                                 |
| `UISlider`     | (NEW) Draggable horizontal slider, value range configurable. |
| `UIMusicPlayer`| (NEW) Composite: panel + icon + title + 4 transport buttons + bar + time label. |

Screens (`include/Game/UI/Menus/`):

| Screen              | New? | Role                                          |
| ------------------- | :--: | --------------------------------------------- |
| `UILoadingScreen`   | —    | The progress bar shown during loading.        |
| `InGameUI`          | mod. | HUD: title, tool buttons, status, pause button, music player. |
| `TitleScreen`       | ✓    | Boot screen — Play / Settings / Credits / Quit. |
| `LevelSelectScreen` | ✓    | Grid of level cards.                          |
| `PauseMenu`         | ✓    | Modal overlay during gameplay.                |
| `SettingsScreen`    | ✓    | Volume sliders + fullscreen/vsync toggles.    |
| `CreditsScreen`     | ✓    | Static credit list.                           |
| `VictoryScreen`     | ✓    | End-of-level success (plus "Win sound").      |
| `GameOverScreen`    | ✓    | End-of-level failure.                         |

### 3.7 `Game::Settings`

- **`GameSettings`** struct: master/sound/music volumes (0-100), fullscreen, vsync, last track + position.
- **`SettingsManager`** singleton: `Load() / Save() / Apply()` via glaze JSON. Default path `assets/Settings/settings.json`.

### 3.8 `Game::AssetDatabase<T>`

Templated singleton-derived caches keyed by relative path. Each `Load(path)` increments a refcount; `Unload(path)` decrements and frees on zero. Implementations:

| Database          | Asset type     | Source folder         |
| ----------------- | -------------- | --------------------- |
| `TextureDatabase` | `sf::Texture`  | `assets/Textures/`    |
| `FontDatabase`    | `sf::Font`     | `assets/Fonts/`       |
| `SoundDatabase`   | `sf::SoundBuffer` | `assets/Sounds/`   |
| `MusicDatabase`   | `sf::Music`    | `assets/Musics/`      |
| `MapDatabase`     | `MapData`      | `assets/Maps/` (glaze) |
| `TileDatabase`    | `TileData`     | `assets/Tiles/` (glaze) |
| `TrackDatabase`   | `TrackData`    | `assets/Tracks/`      |
| `StationDatabase` | `StationData`  | `assets/Stations/`    |
| `TrainDatabase`   | `TrainData`    | `assets/Trains/`      |

`DatabasePath` is `current_path() / "assets"`, so all relative paths use Windows backslash separators (`Tiles\\water.json`, `Textures\\grass_block_top.png`, …).

### 3.9 `PallasEngine`

Minimal in-house logger (file + console). Used everywhere via `pallas::Logger m_Logger`. Performance logger is enabled in DEBUG.

---

## 4. Asset details

### 4.1 Maps (`assets/Maps/`)

Generated procedurally with 2-layer smooth value noise (elevation + humidity) + sculpted lakes:

| Map                 |    Size |   Tiles | Biome    | Playlist                | Notes                                         |
| ------------------- | -------:| -------:| -------- | ----------------------- | --------------------------------------------- |
| `DEFAULT`           |   32×32 |   1 024 | Default  | `Musics\1.Night_Drive`  | Fallback / sandbox.                           |
| `Plain`             |   80×80 |   6 400 | Plain    | `Musics\3.Lo-Fi_Journey`| Meadow + 6 small lakes.                       |
| `ForestValley`      |   80×80 |   6 400 | Forest   | `Musics\4.Zen`          | Meandering river with 3 gravel fords.         |
| `FrozenHighlands`   |   96×96 |   9 216 | Frozen   | `Musics\4.Zen`          | 6 frozen lakes (4 with snow crossings).       |
| `DesertCanyon`      |   96×64 |   6 144 | Desert   | `Musics\3.Lo-Fi_Journey`| Sand → red sand → terracotta canyons.         |
| `VolcanicWastes`    |   80×80 |   6 400 | Volcanic | `Musics\2.Industrial_Ambiant` | Lava lakes + magma + nether wart patches. |
| `NeonCity`          |   64×64 |   4 096 | Neon     | `Musics\5.Neons_n_Rain` | 7 Voronoi districts + central plaza.          |
| **Total**           |         | **39 680** |          |                         |                                               |

All maps are **guaranteed single-connected** for buildable tiles — small isolated pockets are absorbed into surrounding blockers so no station can land on an unreachable island.

### 4.2 Tiles (`assets/Tiles/`)

44 tile definitions. Each tile JSON references a texture path, color tint, size, animation flags, and a `m_CanBuild` bool. The 7 blocking tiles for rails are: `water`, `lava`, `magma`, `ice`, `blue_ice`, `packed_ice`, `powder_snow`.

### 4.3 Textures (`assets/Textures/`)

46 top-down or visually-neutral block textures sourced from a vanilla Minecraft pack (`assets/Textures/.IDK/block/`). Foliage / side-view trunks were intentionally removed to avoid transparency artifacts on top-down tiles.

### 4.4 UI (`assets/UI/`)

77 PNG sprites in 14 themed subfolders:

```
Buttons/        — normal/hover/disabled (basic, close, locked, unlocked, warning)
Sliders/        — track + handle, normal/hover
Checkboxes/     — off/on, normal/hover
Scroll/         — thumb + track
Tabs/           — normal/hover/selected/selected_hover
TextField/      — normal/focus
MusicControls/  — prev/play/pause/next/stop, normal/hover (10 PNG)
Panels/         — popup_bg, window_bg, panel_stone, panel_adventure
LevelSelect/    — join/marked_join/warning + sort_up/down
Bars/           — boss-bar style progress, blue/green/purple/notched
Toasts/         — now_playing, system, advancement
Tooltips/       — background + frame
Icons/          — music, check, info, accessibility, language, search
Statistics/     — header
```

Play / Pause / Stop were generated as 16×16 pixel-art icons (the vanilla pack has no media-control set).

### 4.5 Music (`assets/Musics/`)

6 playlists totalling 44 tracks:

| Playlist                  | Tracks | Used by              |
| ------------------------- | ------:| -------------------- |
| `0.Menus`                 |     12 | All menus            |
| `1.Night_Drive`           |      7 | DEFAULT              |
| `2.Industrial_Ambiant`    |      7 | VolcanicWastes       |
| `3.Lo-Fi_Journey`         |      7 | Plain, DesertCanyon  |
| `4.Zen`                   |      6 | FrozenHighlands, ForestValley |
| `5.Neons_n_Rain`          |      5 | NeonCity             |

`AudioManager` shuffles each playlist on load and re-shuffles at the loop boundary.

### 4.6 Screen flow

```
              ┌──────────────┐
              │  TitleScreen │
              └──────┬───────┘
        Play         │ Settings    Credits        Quit
        ▼            ▼             ▼              ▼
┌──────────────┐ ┌──────────┐ ┌─────────┐  (graceful exit)
│ LevelSelect  │ │ Settings │ │ Credits │
└──────┬───────┘ └────┬─────┘ └────┬────┘
       │ pick a map  │ back       │ back
       ▼             ▼            ▼
┌──────────────┐  back to Title (or Pause if reached from Pause)
│   InGameUI   │◄────────────────────────────────────┐
└──────┬───────┘                                     │
       │ pause btn                Win                │ Lose
       ▼                           ▼                  ▼
┌──────────────┐  Resume     ┌──────────┐       ┌────────────┐
│  PauseMenu   │────────────►│ Victory  │       │ GameOver   │
└──────┬───────┘             └────┬─────┘       └─────┬──────┘
       │ Main Menu / Quit         │ Replay/Next/Menu  │ Retry/Menu
       └──────────────────────────┴───────────────────┘
```

---

## 5. Build & run

1. Open `MineRail Valley.sln` in **Visual Studio 2022**.
2. Select `x64 Debug` or `x64 Release`.
3. **Build → Rebuild Solution**.
4. The post-build event copies SFML DLLs and the `assets/` tree into the output directory.
5. F5 to run.

The Release build switches `<SubSystem>Console</SubSystem>` to `Windows` and sets the entry point to `mainCRTStartup`, so the game opens without a console.

---

## 6. Coding conventions

- **C++23**. `stdcpp23` is set in the vcxproj.
- All gameplay code is in `namespace Game`.
- Singletons via `static T& Instance() { static T inst; return inst; }`.
- Heap-owned UI widgets — each screen `delete`s its children via the base `UIElement::~UIElement()` traversal.
- Asset paths use Windows backslashes in the strings: `"Tiles\\grass.json"`.
- glaze JSON for any persistent struct.
- Logging via the in-house `pallas::Logger`.

---

## 7. Known issues / fragile points

- **Filesystem sync between WSL/VM and Windows host occasionally truncates JSON files when written from the Linux side.** All map / tile generators in this project use a robust write helper (write to `/tmp`, copy, verify size, retry). The files on the Windows host (`C:\Users\…\MineRail-Valley`) are intact; only Linux-side reads sometimes appear truncated until the host syncs.
- **Slider release-outside detection** uses `sf::Mouse::isButtonPressed` in `UISlider::Update` — works but relies on the SFML mouse polling API, which is global. Multi-window setups would break this.
- **Fullscreen toggle**: applied only on app start (from `settings.Fullscreen`). Live toggling in Settings requires a dedicated `GameWindow::SetFullscreen(bool)` API that handles the "already fullscreen → ignore" case.

---

## 8. What's done vs what's left

### Done
- **World gen**: 7 maps with smooth biome transitions, lakes, deliberate crossing points, single-connectivity guarantee.
- **Audio**: playlist system (shuffle + auto-next), menu/in-game biome-aware switching, transport API.
- **UI**: full screen stack (Title, LevelSelect, Pause, Settings, Credits, Victory, GameOver, HUD with embedded music player), 4 new generic widgets + 1 composite.
- **Settings**: persistence to `settings.json`, applied on boot, reapplied on Settings.Apply.
- **GameManager**: complete screen flow with proper level lifecycle and music transitions.
- **SFX**: menu click + win sound wired.

### Pending — UI / polish
- [ ] **Keyboard Pause shortcut** (ESC → ShowPause). Currently you must click the pause button. Hook in `InputManager`.
- [ ] **Live fullscreen toggle** in Settings (see §7).
- [ ] **Scroll** for long screens — only the Credits would benefit; could use `assets/UI/Scroll/`.
- [ ] **Hover SFX** for buttons (subtle "tick").
- [ ] **Tooltip widget** for hovering on level cards / settings — assets are already in `assets/UI/Tooltips/`.
- [ ] **Now-Playing toast** when a track changes — assets ready in `assets/UI/Toasts/now_playing.png`.

### Pending — Gameplay loop
- [ ] **Win condition** wiring: when the player completes a level (e.g. all stations served), call `GameManager::Instance().ShowVictory(stats)`.
- [ ] **Lose condition** wiring: when the player fails (money out, deadline), call `ShowGameOver(reason)`.
- [ ] **Score / time tracking** so VictoryScreen has real numbers to show.
- [ ] **Level lock progression** — `LevelSelectScreen::LevelInfo::locked` is supported by the UI; needs a save file tracking completed levels.
- [ ] **Train SFX wiring**: `TrainSystem` should emit `PlaySoundEvent{"Sounds\\train-move.mp3"}` while moving, `train-whistle.mp3` on station passage, `train-crash.mp3` on collision.

### Pending — Engine
- [ ] **9-slice rendering** for `UIPanel` so panel backgrounds keep crisp corners when stretched. Files in `assets/UI/Panels/` are designed for it.
- [ ] **Animated tile support** in the renderer (the data model already has `animated/framesX/framesY/duration` — see `lava.json`).
- [ ] **`World::Pause`** could optionally also pause the music; currently music keeps playing in the Pause menu (which is intentional, but a Settings option to mute would be nice).

### Pending — Audio
We currently ship 5 SFX. The recommended additions are listed below. If you want me to generate them (synthesised) or look for public-domain alternatives, let me know:

| Missing SFX           | Used when                                          |
| --------------------- | -------------------------------------------------- |
| `ui_hover.wav`        | Mouse enters a UIButton                            |
| `ui_deny.wav`         | Clicking a locked level or disabled action         |
| `pause_in.wav`        | Entering pause                                     |
| `pause_out.wav`       | Resuming                                           |
| `track_place.wav`     | Placing a rail tile                                |
| `track_remove.wav`    | Bulldozing a tile                                  |
| `station_built.wav`   | New station placed                                 |
| `station_connected.wav` | Two stations linked                              |
| `loading_complete.wav`| Right after `LoadingComplete` fires                |
| `money_earned.wav`    | Player earns money                                 |
| `train_arrive.wav`    | Train enters station                               |
| `train_depart.wav`    | Train leaves station                               |
| `game_over.wav`       | Counterpart of the existing `Win sound.wav`        |

### Pending — Content
- [ ] **Tutorial / first-time-user flow**: a guided overlay on `DEFAULT` (which is intentionally sandbox-sized 32×32).
- [ ] **More levels**: the current 7 maps cover the 6 themed playlists + a sandbox. Add seasonal variants?
- [ ] **Achievement / stats** persistence (consider extending `GameSettings`).

### Pending — Distribution
- [ ] Add a real game icon.
- [ ] Switch `settings.json` storage to `%APPDATA%\MineRail Valley\settings.json` for proper user-scoped storage in shipped builds.
- [ ] Strip `.IDK/` and `OLD/` folders from the release artifact (post-build script ships everything in `assets/`).

---

## 9. Quick wiring cookbook

```cpp
// app boot
int main() { return Game::GameManager::Instance().Start(); }

// transitions, somewhere inside the game
GameManager::Instance().ShowTitle();
GameManager::Instance().ShowLevelSelect();
GameManager::Instance().LoadLevel("Maps\\Plain.json");
GameManager::Instance().ShowPause();
GameManager::Instance().ShowVictory({{"Score", "1240"}, {"Time", "3:14"}});
GameManager::Instance().RequestQuit();

// fire a sound from anywhere
AudioManager::Instance().PlaySound("Sounds\\Menu Selection Click.wav", 0.8f);

// read current settings
const auto& s = SettingsManager::Instance().Get();
```

For the full step-by-step screen-by-screen wiring guide, see
[`UI_INTEGRATION.md`](./UI_INTEGRATION.md).
