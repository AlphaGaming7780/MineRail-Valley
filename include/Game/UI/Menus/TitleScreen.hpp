#pragma once
#include <Game/UI.hpp>

namespace Game
{
    // TitleScreen — the very first UI shown on app start.
    //
    // Layout:
    //   ┌─────────────────────────────────┐
    //   │         MINERAIL VALLEY         │      (large title)
    //   │                                 │
    //   │            [  Play  ]           │
    //   │            [Settings]           │
    //   │            [Credits ]           │
    //   │            [  Quit  ]           │
    //   │                                 │
    //   │ v0.1                       ♪    │      (version + tiny music toast)
    //   └─────────────────────────────────┘
    //
    // Wiring TODO (after construction, e.g. in GameManager):
    //   auto& t = UIManager::Instance().SetRoot<TitleScreen>();
    //   t.SetOnPlay    ([] { ShowLevelSelect(); });
    //   t.SetOnSettings([] { ShowSettings(/*returnTo=*/Title); });
    //   t.SetOnCredits ([] { ShowCredits(); });
    //   t.SetOnQuit    ([] { /* request graceful shutdown */ });
    //
    // Audio: emit a PlayMusicEvent / PlayPlaylist for "Musics\\0.Menus" when
    //        this screen becomes active (or rely on LoadingComplete with
    //        GameMode::MainMenu — see AudioManager.cpp).
    class TitleScreen : public UIGroup
    {
    public:
        TitleScreen();
        virtual ~TitleScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        void SetOnPlay    (std::function<void()> cb) { m_OnPlay     = std::move(cb); }
        void SetOnSettings(std::function<void()> cb) { m_OnSettings = std::move(cb); }
        void SetOnCredits (std::function<void()> cb) { m_OnCredits  = std::move(cb); }
        void SetOnQuit    (std::function<void()> cb) { m_OnQuit     = std::move(cb); }

    private:
        sf::Font*    m_Font            = nullptr;
        sf::Texture* m_BtnTex          = nullptr;
        sf::Texture* m_BtnHoverTex     = nullptr;
        sf::Texture* m_BtnClickedTex   = nullptr;
        sf::Texture* m_WarnTex         = nullptr;
        sf::Texture* m_WarnHoverTex    = nullptr;

        UILabel*  m_TitleLabel    = nullptr;
        UILabel*  m_VersionLabel  = nullptr;
        UIButton* m_PlayButton    = nullptr;
        UIButton* m_SettingsButton= nullptr;
        UIButton* m_CreditsButton = nullptr;
        UIButton* m_QuitButton    = nullptr;

        std::function<void()> m_OnPlay;
        std::function<void()> m_OnSettings;
        std::function<void()> m_OnCredits;
        std::function<void()> m_OnQuit;
    };
}
