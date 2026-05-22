#pragma once
#include <Game/UI.hpp>
#include <Game/UI/Widgets/UIMusicPlayer.hpp>

namespace Game
{
    // PauseMenu — overlay shown when the player pauses the game.
    //
    // Renders a darkened backdrop over the existing HUD/world frame (the
    // backdrop is a UIPanel with BlocksMouse=true so clicks behind don't
    // leak through). Embeds a fuller-width UIMusicPlayer so the player can
    // skip tracks without leaving Pause.
    //
    // Wiring TODO:
    //   pause->SetOnResume    ([] { /* set GameState back to WorldReady */ });
    //   pause->SetOnSettings  ([] { /* push SettingsScreen; remember returnTo=Pause */ });
    //   pause->SetOnMainMenu  ([] { /* tear down level + SetRoot<TitleScreen> */ });
    //   pause->SetOnQuit      ([] { /* graceful shutdown */ });
    //   pause->WireMusicPlayer(audioManager);  // see UI_INTEGRATION.md
    class PauseMenu : public UIGroup
    {
    public:
        PauseMenu();
        virtual ~PauseMenu();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        UIMusicPlayer* GetMusicPlayer() { return m_MusicPlayer; }

        void SetOnResume   (std::function<void()> cb) { m_OnResume   = std::move(cb); }
        void SetOnSettings (std::function<void()> cb) { m_OnSettings = std::move(cb); }
        void SetOnMainMenu (std::function<void()> cb) { m_OnMainMenu = std::move(cb); }
        void SetOnQuit     (std::function<void()> cb) { m_OnQuit     = std::move(cb); }

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_WarnTex       = nullptr;
        sf::Texture* m_WarnHoverTex  = nullptr;

        UIPanel*  m_Backdrop      = nullptr;
        UIPanel*  m_DialogPanel   = nullptr;
        UILabel*  m_TitleLabel    = nullptr;
        UIButton* m_ResumeButton  = nullptr;
        UIButton* m_SettingsButton= nullptr;
        UIButton* m_MainMenuButton= nullptr;
        UIButton* m_QuitButton    = nullptr;
        UIMusicPlayer* m_MusicPlayer = nullptr;

        std::function<void()> m_OnResume;
        std::function<void()> m_OnSettings;
        std::function<void()> m_OnMainMenu;
        std::function<void()> m_OnQuit;
    };
}
