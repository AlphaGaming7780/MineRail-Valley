#pragma once
#include <Game/UI.hpp>

namespace Game
{
    // SettingsScreen — full-screen settings menu accessible from Title or Pause.
    //
    //   ┌─────────────────────────────────────────────────────┐
    //   │ Settings                                            │
    //   │                                                     │
    //   │  Master Volume   ▰▰▰▰▰▰▰▱▱▱▱▱▱  70%                 │
    //   │  Sound Volume    ▰▰▰▰▰▰▰▰▰▱▱▱  85%                  │
    //   │  Music Volume    ▰▰▰▰▰▰▱▱▱▱▱▱  60%                  │
    //   │                                                     │
    //   │  Fullscreen      [x]                                │
    //   │  VSync           [ ]                                │
    //   │                                                     │
    //   │              [ Apply ]   [ Back ]                   │
    //   └─────────────────────────────────────────────────────┘
    //
    // The screen does NOT load/save the GameSettings struct itself — it just
    // exposes the current values via getters and invokes m_OnApply when the
    // user clicks Apply. Persist/restore from your GameManager.
    //
    // Wiring TODO:
    //   settings->SetCurrent({ /* GameSettings from disk */ });
    //   settings->SetOnApply([] (const GameSettings& s) { SaveAndApply(s); });
    //   settings->SetOnBack ([] { /* pop screen */ });
    struct GameSettings;  // fwd-declared, defined in Game/Settings/GameSettings.hpp
    class SettingsScreen : public UIGroup
    {
    public:
        SettingsScreen();
        virtual ~SettingsScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Push the current settings values into the widgets.
        void SetCurrent(const GameSettings& s);
        // Read the current widget values into a GameSettings.
        void ReadInto(GameSettings& out) const;

        // m_OnApply receives the live widget state. Implementer should:
        //   1) Apply to AudioManager (SetMasterVolume, SetMusicVolume, ...)
        //   2) Apply to RenderingManager / Window (fullscreen, vsync)
        //   3) Serialize to assets/Settings/settings.json
        void SetOnApply(std::function<void(const GameSettings&)> cb) { m_OnApply = std::move(cb); }
        void SetOnBack (std::function<void()> cb)                    { m_OnBack  = std::move(cb); }

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_TrackTex      = nullptr;
        sf::Texture* m_HandleTex     = nullptr;
        sf::Texture* m_CbOffTex      = nullptr;
        sf::Texture* m_CbOffHoverTex = nullptr;
        sf::Texture* m_CbOnTex       = nullptr;
        sf::Texture* m_CbOnHoverTex  = nullptr;
        sf::Texture* m_PanelTex      = nullptr;

        UILabel*  m_TitleLabel = nullptr;

        UILabel*  m_MasterLabel = nullptr;  UISlider* m_MasterSlider = nullptr;  UILabel* m_MasterValueLabel = nullptr;
        UILabel*  m_SoundLabel  = nullptr;  UISlider* m_SoundSlider  = nullptr;  UILabel* m_SoundValueLabel  = nullptr;
        UILabel*  m_MusicLabel  = nullptr;  UISlider* m_MusicSlider  = nullptr;  UILabel* m_MusicValueLabel  = nullptr;

        UILabel*  m_FullscreenLabel = nullptr;  UIToggle* m_FullscreenToggle = nullptr;
        UILabel*  m_VSyncLabel      = nullptr;  UIToggle* m_VSyncToggle      = nullptr;

        UIButton* m_ApplyButton = nullptr;
        UIButton* m_BackButton  = nullptr;

        std::function<void(const GameSettings&)> m_OnApply;
        std::function<void()> m_OnBack;
    };
}
