#include <Game/UI/Menus/PauseMenu.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    PauseMenu::PauseMenu()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button.png");
        m_BtnHoverTex   = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");
        m_WarnTex       = tx.Load("UI\\Buttons\\warning_normal.png");
        m_WarnHoverTex  = tx.Load("UI\\Buttons\\warning_hover.png");

        // Full-screen darken backdrop that blocks clicks reaching the HUD.
        m_Backdrop = new UIPanel(nullptr, { 0, 0 }, { 0, 0 }, UIAnchor::TopLeft,
                                 sf::Color(0, 0, 0, 160));
        m_Backdrop->SetBlocksMouse(true);
        AddChild(m_Backdrop);

        // Dialog box (centered, resized in UpdateLayout)
        m_DialogPanel = new UIPanel(nullptr, { 0, 0 }, { 420, 360 }, UIAnchor::TopLeft,
                                    sf::Color(30, 30, 35, 235));
        AddChild(m_DialogPanel);

        m_TitleLabel = new UILabel("Paused", { 0, 30 }, UIAnchor::TopLeft,
                                   m_Font, 40, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        const sf::Vector2f BTN_SZ{ 280, 50 };
        m_ResumeButton   = new UIButton({ 0, 110 }, BTN_SZ, UIAnchor::TopLeft,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_SettingsButton = new UIButton({ 0, 170 }, BTN_SZ, UIAnchor::TopLeft,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_MainMenuButton = new UIButton({ 0, 230 }, BTN_SZ, UIAnchor::TopLeft,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_QuitButton     = new UIButton({ 0, 290 }, BTN_SZ, UIAnchor::TopLeft,
                                        m_WarnTex, m_WarnHoverTex, m_WarnTex);

        m_ResumeButton  ->SetCallback([this] { if (m_OnResume)   m_OnResume();   });
        m_SettingsButton->SetCallback([this] { if (m_OnSettings) m_OnSettings(); });
        m_MainMenuButton->SetCallback([this] { if (m_OnMainMenu) m_OnMainMenu(); });
        m_QuitButton    ->SetCallback([this] { if (m_OnQuit)     m_OnQuit();     });

        auto lbl = [&](const std::string& t, UIButton* b) {
            b->AddChild(new UILabel(t, { 0, 0 }, UIAnchor::Center,
                                    m_Font, 22, { 243, 231, 207 }));
        };
        lbl("Resume",     m_ResumeButton);
        lbl("Settings",   m_SettingsButton);
        lbl("Main Menu",  m_MainMenuButton);
        lbl("Quit Game",  m_QuitButton);

        AddChild(m_ResumeButton);
        AddChild(m_SettingsButton);
        AddChild(m_MainMenuButton);
        AddChild(m_QuitButton);

        // Music player at the top-right; takes its default 280x90 size.
        m_MusicPlayer = new UIMusicPlayer({ 16, 16 }, { 280, 90 }, UIAnchor::TopRight);
        AddChild(m_MusicPlayer);
    }

    PauseMenu::~PauseMenu()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        tx.Unload("UI\\Buttons\\warning_normal.png");
        tx.Unload("UI\\Buttons\\warning_hover.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void PauseMenu::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        if (m_Backdrop) m_Backdrop->SetSize(m_Size);

        // Center the dialog box
        const sf::Vector2f dlgSz{ 420, 360 };
        sf::Vector2f dlgPos{ (m_Size.x - dlgSz.x) * 0.5f, (m_Size.y - dlgSz.y) * 0.5f };
        if (m_DialogPanel) m_DialogPanel->SetRelativePosition(dlgPos);

        // Title centered above the buttons
        if (m_TitleLabel)
            m_TitleLabel->SetRelativePosition({ dlgPos.x + dlgSz.x * 0.5f - 70, dlgPos.y + 30 });

        // Buttons aligned to the dialog
        const float btnX = dlgPos.x + (dlgSz.x - 280) * 0.5f;
        if (m_ResumeButton)   m_ResumeButton  ->SetRelativePosition({ btnX, dlgPos.y + 110 });
        if (m_SettingsButton) m_SettingsButton->SetRelativePosition({ btnX, dlgPos.y + 170 });
        if (m_MainMenuButton) m_MainMenuButton->SetRelativePosition({ btnX, dlgPos.y + 230 });
        if (m_QuitButton)     m_QuitButton    ->SetRelativePosition({ btnX, dlgPos.y + 290 });

        UIGroup::UpdateLayout(view);
    }

    void PauseMenu::Draw(sf::RenderTexture& canvas)
    {
        // Don't clear — the backdrop UIPanel handles the dimming effect.
        UIGroup::Draw(canvas);
    }
}
