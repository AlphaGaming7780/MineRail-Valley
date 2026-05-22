#include <Game/UI/Menus/SettingsScreen.hpp>
#include <Game/Settings/GameSettings.hpp>
#include <Game/UI/Menus/TitleScreen.hpp>
#include <Game/AudioManager.hpp>
#include <Game/Rendering/GameWindow.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    SettingsScreen::SettingsScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button.png");
        m_BtnHoverTex   = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");
        m_TrackTex      = tx.Load("UI\\Sliders\\track_normal.png");
        m_HandleTex     = tx.Load("UI\\Sliders\\handle_normal.png");
        m_CbOffTex      = tx.Load("UI\\Checkboxes\\off_normal.png");
        m_CbOffHoverTex = tx.Load("UI\\Checkboxes\\off_hover.png");
        m_CbOnTex       = tx.Load("UI\\Checkboxes\\on_normal.png");
        m_CbOnHoverTex  = tx.Load("UI\\Checkboxes\\on_hover.png");
        m_PanelTex      = tx.Load("UI\\Panels\\popup_bg.png");

        m_TitleLabel = new UILabel("Settings", { 24, 16 }, UIAnchor::TopLeft,
                                   m_Font, 40, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        // ── Volume sliders ─────────────────────────────────────────────────
        auto addSlider = [&](const std::string& label,
            float y,
            UILabel*& outLbl,
            UISlider*& outSld,
            UILabel*& outVal,
            float init,
            std::function<void(float)> onChange)
            {
                outLbl = new UILabel(label, { 80, y }, UIAnchor::TopLeft,
                    m_Font, 22, { 236, 222, 190 });

                outSld = new UISlider(m_TrackTex, m_HandleTex,
                    { 300, y }, { 320, 24 }, UIAnchor::TopLeft,
                    0.f, 100.f, init);

                outVal = new UILabel("0", { 640, y }, UIAnchor::TopLeft,
                    m_Font, 22, { 207, 182, 151 });

                outSld->SetCallback([outVal, onChange](float v)
                    {
                        outVal->SetText(std::to_string(static_cast<int>(v)));

                        if (onChange)
                            onChange(v);
                    });

                AddChild(outLbl);
                AddChild(outSld);
                AddChild(outVal);
            };


        addSlider("Master Volume", 110, m_MasterLabel, m_MasterSlider, m_MasterValueLabel, 100, [](float v) { AudioManager::Instance().SetMasterVolume(v); });
        addSlider("Sound Volume", 160, m_SoundLabel, m_SoundSlider, m_SoundValueLabel, 100, [](float v) { AudioManager::Instance().SetSoundVolume(v); });
        addSlider("Music Volume", 210, m_MusicLabel, m_MusicSlider, m_MusicValueLabel, 100, [](float v) { AudioManager::Instance().SetMusicVolume(v); });

        // ── Toggles ────────────────────────────────────────────────────────
        m_FullscreenLabel = new UILabel("Fullscreen", { 80, 290 }, UIAnchor::TopLeft,
                                        m_Font, 22, { 236, 222, 190 });
        m_FullscreenToggle = new UIToggle({ 300, 290 }, { 24, 24 }, UIAnchor::TopLeft,
                                          m_CbOffTex, m_CbOffHoverTex, m_CbOnTex, m_CbOnHoverTex);
        m_FullscreenToggle->SetChecked(GameWindow::Instance().IsFullscreen());
        m_FullscreenToggle->SetCallback([](bool v) { GameWindow::Instance().ToggleFullscreen(v); });
        AddChild(m_FullscreenLabel);
        AddChild(m_FullscreenToggle);

        m_VSyncLabel = new UILabel("VSync", { 80, 330 }, UIAnchor::TopLeft,
                                   m_Font, 22, { 236, 222, 190 });
        m_VSyncToggle = new UIToggle({ 300, 330 }, { 24, 24 }, UIAnchor::TopLeft,
                                     m_CbOffTex, m_CbOffHoverTex, m_CbOnTex, m_CbOnHoverTex);
        AddChild(m_VSyncLabel);
        AddChild(m_VSyncToggle);

        // ── Action buttons ─────────────────────────────────────────────────
        m_ApplyButton = new UIButton({ 80, 60 }, { 180, 48 }, UIAnchor::BottomLeft,
                                     m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_ApplyButton->SetCallback([this] {
            if (m_OnApply) {
                GameSettings s; ReadInto(s);
                m_OnApply(s);
            }
        });
        m_ApplyButton->AddChild(new UILabel("Apply", { 0, 0 }, UIAnchor::Center,
                                            m_Font, 22, { 243, 231, 207 }));
        AddChild(m_ApplyButton);

        m_BackButton = new UIButton({ 280, 60 }, { 180, 48 }, UIAnchor::BottomLeft,
                                    m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_BackButton->SetCallback([this] { UIManager::Instance().RequestNewRoot<TitleScreen>(); });
        m_BackButton->AddChild(new UILabel("Back", { 0, 0 }, UIAnchor::Center,
                                           m_Font, 22, { 243, 231, 207 }));
        AddChild(m_BackButton);
    }

    SettingsScreen::~SettingsScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        tx.Unload("UI\\Sliders\\track_normal.png");
        tx.Unload("UI\\Sliders\\handle_normal.png");
        tx.Unload("UI\\Checkboxes\\off_normal.png");
        tx.Unload("UI\\Checkboxes\\off_hover.png");
        tx.Unload("UI\\Checkboxes\\on_normal.png");
        tx.Unload("UI\\Checkboxes\\on_hover.png");
        tx.Unload("UI\\Panels\\popup_bg.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void SettingsScreen::SetCurrent(const GameSettings& s)
    {
        if (m_MasterSlider) m_MasterSlider->SetValue(s.MasterVolume);
        if (m_SoundSlider)  m_SoundSlider ->SetValue(s.SoundVolume);
        if (m_MusicSlider)  m_MusicSlider ->SetValue(s.MusicVolume);
        if (m_FullscreenToggle) m_FullscreenToggle->SetChecked(s.Fullscreen);
        if (m_VSyncToggle)      m_VSyncToggle     ->SetChecked(s.VSync);
    }

    void SettingsScreen::ReadInto(GameSettings& out) const
    {
        if (m_MasterSlider) out.MasterVolume = m_MasterSlider->GetValue();
        if (m_SoundSlider)  out.SoundVolume  = m_SoundSlider ->GetValue();
        if (m_MusicSlider)  out.MusicVolume  = m_MusicSlider ->GetValue();
        if (m_FullscreenToggle) out.Fullscreen = m_FullscreenToggle->IsChecked();
        if (m_VSyncToggle)      out.VSync      = m_VSyncToggle     ->IsChecked();
    }

    void SettingsScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        UIGroup::UpdateLayout(view);
    }

    void SettingsScreen::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear({ 24, 26, 30 });
        UIGroup::Draw(canvas);
    }
}
