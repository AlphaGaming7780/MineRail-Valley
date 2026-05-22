#include <Game/UI/Widgets/UIMusicPlayer.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    UIMusicPlayer::UIMusicPlayer(sf::Vector2f position, sf::Vector2f size, UIAnchor anchor)
    {
        m_RelPosition = position;
        m_Size        = size;
        m_Anchor      = anchor;

        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        // ── Assets ─────────────────────────────────────────────────────────
        m_Font           = fn.Load("Fonts\\Minecraft.ttf");
        m_PanelTex       = tx.Load("UI\\Panels\\popup_bg.png");

        m_MusicIconTex   = tx.Load("UI\\Icons\\music.png");

        m_PrevTex        = tx.Load("UI\\MusicControls\\prev_normal.png");
        m_PrevHoverTex   = tx.Load("UI\\MusicControls\\prev_hover.png");
        m_NextTex        = tx.Load("UI\\MusicControls\\next_normal.png");
        m_NextHoverTex   = tx.Load("UI\\MusicControls\\next_hover.png");
        m_PlayTex        = tx.Load("UI\\MusicControls\\play_normal.png");
        m_PlayHoverTex   = tx.Load("UI\\MusicControls\\play_hover.png");
        m_PauseTex       = tx.Load("UI\\MusicControls\\pause_normal.png");
        m_PauseHoverTex  = tx.Load("UI\\MusicControls\\pause_hover.png");
        m_StopTex        = tx.Load("UI\\MusicControls\\stop_normal.png");
        m_StopHoverTex   = tx.Load("UI\\MusicControls\\stop_hover.png");

        m_BarBgTex       = tx.Load("UI\\Bars\\blue_bg.png");
        m_BarFgTex       = tx.Load("UI\\Bars\\blue_fg.png");

        // ── Background panel ───────────────────────────────────────────────
        m_Background = new UIPanel(m_PanelTex, { 0, 0 }, m_Size, UIAnchor::TopLeft);
        AddChild(m_Background);

        // ── Title line (icon + label) ──────────────────────────────────────
        m_MusicIcon  = new UIImage(m_MusicIconTex, { 8, 8 }, { 18, 18 }, UIAnchor::TopLeft);
        AddChild(m_MusicIcon);

        m_TitleLabel = new UILabel("— No track —", { 32, 6 }, UIAnchor::TopLeft,
                                   m_Font, 16, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        // ── Transport buttons (Prev / PlayPause / Next / Stop) ─────────────
        const sf::Vector2f BTN_SZ{ 30, 30 };
        const float btn_y = 32.f;

        m_PrevButton = new UIButton({ 10,  btn_y }, BTN_SZ, UIAnchor::TopLeft,
                                    m_PrevTex, m_PrevHoverTex, m_PrevTex);
        m_PrevButton->SetCallback([this]() { if (m_OnPrev) m_OnPrev(); });
        AddChild(m_PrevButton);

        m_PlayButton = new UIButton({ 50,  btn_y }, BTN_SZ, UIAnchor::TopLeft,
                                    m_PlayTex, m_PlayHoverTex, m_PlayTex);
        m_PlayButton->SetCallback([this]() {
            if (m_OnPlayPause) m_OnPlayPause();
            SetPlaying(!m_Playing); // visually toggle; the real state should be
                                    // re-pushed by the host via SetPlaying() if
                                    // the AudioManager refuses the request.
        });
        AddChild(m_PlayButton);

        m_NextButton = new UIButton({ 90,  btn_y }, BTN_SZ, UIAnchor::TopLeft,
                                    m_NextTex, m_NextHoverTex, m_NextTex);
        m_NextButton->SetCallback([this]() { if (m_OnNext) m_OnNext(); });
        AddChild(m_NextButton);

        m_StopButton = new UIButton({ 130, btn_y }, BTN_SZ, UIAnchor::TopLeft,
                                    m_StopTex, m_StopHoverTex, m_StopTex);
        m_StopButton->SetCallback([this]() { if (m_OnStop) m_OnStop(); });
        AddChild(m_StopButton);

        // ── Progress bar + time label ──────────────────────────────────────
        // We use UIProgressBar with bar textures from assets/UI/Bars/. The
        // existing UIProgressBar expects (frame, bar, endCap). We don't have
        // an end cap in the boss_bar set, so we pass the bg texture as a poor-
        // man's endCap — visually it's a 1-pixel gradient terminator.
        m_Progress = new UIProgressBar(m_BarBgTex, m_BarFgTex, m_BarBgTex,
                                       { 10, 70 }, { m_Size.x - 60, 8 },
                                       UIAnchor::TopLeft);
        m_Progress->SetValue(0.f);
        AddChild(m_Progress);

        m_TimeLabel = new UILabel("0:00 / 0:00",
                                  { 10, 70 }, UIAnchor::TopRight,
                                  m_Font, 12, { 207, 182, 151 });
        AddChild(m_TimeLabel);
    }

    UIMusicPlayer::~UIMusicPlayer()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        tx.Unload("UI\\Panels\\popup_bg.png");
        tx.Unload("UI\\Icons\\music.png");
        tx.Unload("UI\\MusicControls\\prev_normal.png");
        tx.Unload("UI\\MusicControls\\prev_hover.png");
        tx.Unload("UI\\MusicControls\\next_normal.png");
        tx.Unload("UI\\MusicControls\\next_hover.png");
        tx.Unload("UI\\MusicControls\\play_normal.png");
        tx.Unload("UI\\MusicControls\\play_hover.png");
        tx.Unload("UI\\MusicControls\\pause_normal.png");
        tx.Unload("UI\\MusicControls\\pause_hover.png");
        tx.Unload("UI\\MusicControls\\stop_normal.png");
        tx.Unload("UI\\MusicControls\\stop_hover.png");
        tx.Unload("UI\\Bars\\blue_bg.png");
        tx.Unload("UI\\Bars\\blue_fg.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void UIMusicPlayer::UpdateLayout(const sf::View& view)
    {
        // The composite already sized itself in the ctor. We only re-anchor
        // child positions if the player itself was moved/resized externally.
        UIGroup::UpdateLayout(view);
    }

    void UIMusicPlayer::SetTrackTitle(const std::string& title)
    {
        if (m_TitleLabel) m_TitleLabel->SetText(title);
    }

    void UIMusicPlayer::SetPlaying(bool playing)
    {
        if (m_Playing == playing) return;
        m_Playing = playing;
        RefreshPlayIcon();
    }

    void UIMusicPlayer::RefreshPlayIcon()
    {
        // We delete + reconstruct the play button so its 3 textures get swapped.
        // This is cheap and avoids exposing setters on UIButton.
        if (m_PlayButton == nullptr) return;
        sf::Vector2f pos = m_PlayButton->GetRelativePosition();
        sf::Vector2f sz  = m_PlayButton->GetSize();

        RemoveChild(m_PlayButton);
        delete m_PlayButton;

        if (m_Playing)
            m_PlayButton = new UIButton(pos, sz, UIAnchor::TopLeft,
                                        m_PauseTex, m_PauseHoverTex, m_PauseTex);
        else
            m_PlayButton = new UIButton(pos, sz, UIAnchor::TopLeft,
                                        m_PlayTex, m_PlayHoverTex, m_PlayTex);

        m_PlayButton->SetCallback([this]() {
            if (m_OnPlayPause) m_OnPlayPause();
            SetPlaying(!m_Playing);
        });
        AddChild(m_PlayButton);
        ReDraw();
    }

    void UIMusicPlayer::SetProgress(float p01)
    {
        if (m_Progress) m_Progress->SetValue(p01);
    }

    void UIMusicPlayer::SetTimeLabels(const std::string& current, const std::string& total)
    {
        if (m_TimeLabel) m_TimeLabel->SetText(current + " / " + total);
    }
}
