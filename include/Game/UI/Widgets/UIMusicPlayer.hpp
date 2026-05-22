#pragma once
#include <Game/UI.hpp>
#include <string>

namespace Game
{
    // UIMusicPlayer: composite widget that displays a mini music control panel.
    //
    //  ┌────────────────────────────────────────────┐
    //  │  ♪  Now Playing — Track Title              │
    //  │  ◀     ⏯     ▶                            │
    //  │  ▰▰▰▰▰▰▰▱▱▱▱▱▱▱▱▱▱▱▱▱▱▱  1:32 / 4:18       │
    //  └────────────────────────────────────────────┘
    //
    // The widget exposes a small surface area for the gameplay layer to wire up:
    //   - SetCallbacks(...) lets you bind Prev / PlayPause / Next / Stop to
    //     whatever functions you want (typically AudioManager methods).
    //   - SetTrackTitle("...") updates the label text.
    //   - SetProgress(0..1) updates the bar (no auto-update; you must call it
    //     each frame from your game loop with currentTrackPos / trackDuration).
    //   - SetPlaying(bool) flips the play/pause icon.
    //
    // Mouse interactions are already wired internally to the 3-4 sub-buttons.
    // The screen that hosts the player just needs to set the callbacks once.
    class UIMusicPlayer : public UIGroup
    {
    public:
        // Constructs the player at the given position/size. The default size of
        // 280x90 is a sensible fit for the HUD corner; the Pause menu can pass
        // a larger size for a fuller-width version.
        UIMusicPlayer(
            sf::Vector2f position = { 16, 16 },
            sf::Vector2f size     = { 280, 90 },
            UIAnchor anchor       = UIAnchor::BottomRight
        );

        virtual ~UIMusicPlayer();

        void UpdateLayout(const sf::View& view) override;

        // ─── Callbacks (wire these to AudioManager from your screen code) ───
        void SetOnPrev     (std::function<void()> cb) { m_OnPrev      = std::move(cb); }
        void SetOnPlayPause(std::function<void()> cb) { m_OnPlayPause = std::move(cb); }
        void SetOnNext     (std::function<void()> cb) { m_OnNext      = std::move(cb); }
        void SetOnStop     (std::function<void()> cb) { m_OnStop      = std::move(cb); }

        // ─── State (call these from your game loop) ───
        void SetTrackTitle(const std::string& title);
        void SetPlaying(bool playing);                  // swaps play <-> pause icon
        void SetProgress(float p01);                    // 0..1, drives the progress bar
        void SetTimeLabels(const std::string& current,  // e.g. "1:32"
                           const std::string& total);   // e.g. "4:18"
        void UpdatePlayer();

    private:
        sf::Font*    m_Font          = nullptr;

        // Backgrounds
        sf::Texture* m_PanelTex      = nullptr;
        UIPanel*     m_Background    = nullptr;

        // Now-playing line (music icon + title)
        sf::Texture* m_MusicIconTex  = nullptr;
        UIImage*     m_MusicIcon     = nullptr;
        UILabel*     m_TitleLabel    = nullptr;

        // Transport controls
        sf::Texture* m_PrevTex       = nullptr;  sf::Texture* m_PrevHoverTex = nullptr;
        sf::Texture* m_NextTex       = nullptr;  sf::Texture* m_NextHoverTex = nullptr;
        sf::Texture* m_PlayTex       = nullptr;  sf::Texture* m_PlayHoverTex = nullptr;
        sf::Texture* m_PauseTex      = nullptr;  sf::Texture* m_PauseHoverTex = nullptr;
        sf::Texture* m_StopTex       = nullptr;  sf::Texture* m_StopHoverTex = nullptr;

        UIButton* m_PrevButton  = nullptr;
        UIButton* m_PlayButton  = nullptr;   // toggles between play/pause icon
        UIButton* m_NextButton  = nullptr;
        UIButton* m_StopButton  = nullptr;   // optional, can be hidden later

        // Progress + times
        sf::Texture* m_BarBgTex   = nullptr;
        sf::Texture* m_BarFgTex   = nullptr;
        UIProgressBar* m_Progress = nullptr;
        UILabel*     m_TimeLabel  = nullptr;

        bool         m_Playing    = false;

        // Wired in ctor
        std::function<void()> m_OnPrev;
        std::function<void()> m_OnPlayPause;
        std::function<void()> m_OnNext;
        std::function<void()> m_OnStop;

        void RefreshPlayIcon();
    };
}
