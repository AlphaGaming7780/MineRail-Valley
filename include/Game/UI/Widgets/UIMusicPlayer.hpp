#pragma once
#include <Game/UI.hpp>
#include <string>

namespace Game
{
    // UIMusicPlayer: composite widget that displays a mini music control panel.
    //
    //  ┌────────────────────────────────────────────┐
    //  │  ♪  Now Playing — Track Title              │
    //  │              ◀     ⏯     ▶                 │
    //  │  ▰▰▰▰▰▰▰▱▱▱▱▱▱▱▱▱▱▱▱▱▱▱  1:32 / 4:18       │
    //  └────────────────────────────────────────────┘
    //
    // Three transport buttons (Prev / PlayPause / Next), all wired directly to
    // AudioManager. There used to be a Stop button too but it was redundant
    // with the pause toggle and made the panel feel cluttered, so it was
    // removed.
    //
    // The screen that hosts the player just instantiates it — no wiring needed.
    // The buttons talk to AudioManager directly and UpdatePlayer() (called by
    // AudioManager every second) syncs the title / progress / play-pause icon
    // back from the audio state.
    class UIMusicPlayer : public UIGroup
    {
    public:
        // Constructs the player at the given position/size. The default size of
        // 280x90 is a sensible fit for the HUD corner; the Pause menu can pass
        // a larger size for a fuller-width version.
        UIMusicPlayer(
            sf::Vector2f position = { 16, 16 },
            sf::Vector2f size     = { 280, 110 },
            UIAnchor anchor       = UIAnchor::BottomRight
        );

        virtual ~UIMusicPlayer();

        void UpdateLayout(const sf::View& view) override;

        // ─── Callbacks (optional — buttons call AudioManager directly by default) ───
        void SetOnPrev     (std::function<void()> cb) { m_OnPrev      = std::move(cb); }
        void SetOnPlayPause(std::function<void()> cb) { m_OnPlayPause = std::move(cb); }
        void SetOnNext     (std::function<void()> cb) { m_OnNext      = std::move(cb); }

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

        // Transport controls (Prev / Play-Pause / Next)
        sf::Texture* m_PrevTex       = nullptr;  sf::Texture* m_PrevHoverTex = nullptr;
        sf::Texture* m_NextTex       = nullptr;  sf::Texture* m_NextHoverTex = nullptr;
        sf::Texture* m_PlayTex       = nullptr;  sf::Texture* m_PlayHoverTex = nullptr;
        sf::Texture* m_PauseTex      = nullptr;  sf::Texture* m_PauseHoverTex = nullptr;

        UIButton* m_PrevButton  = nullptr;
        UIButton* m_PlayButton  = nullptr;   // toggles between play/pause icon
        UIButton* m_NextButton  = nullptr;

        // Progress + times
        sf::Texture* m_BarBgTex   = nullptr;
        sf::Texture* m_BarFgTex   = nullptr;
        UIProgressBar* m_Progress = nullptr;
        UILabel*     m_TimeLabel  = nullptr;

        bool         m_Playing    = false;

        // Optional user callbacks (not used by default — buttons go straight to AudioManager)
        std::function<void()> m_OnPrev;
        std::function<void()> m_OnPlayPause;
        std::function<void()> m_OnNext;

        void RefreshPlayIcon();
    };
}
