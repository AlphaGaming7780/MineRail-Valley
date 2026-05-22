#include <Game/UI/Menus/PauseMenu.hpp>
#include <Game/UI/Menus/InGameUI.h>
#include <Game/AssetDatabase.h>
#include <Game/GameManager.hpp>

namespace Game
{
    PauseMenu::PauseMenu()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase& fn = FontDatabase::Instance();

        m_Font = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex = tx.Load("UI\\Button.png");
        m_BtnHoverTex = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");
        m_WarnTex = tx.Load("UI\\Buttons\\warning_normal.png");
        m_WarnHoverTex = tx.Load("UI\\Buttons\\warning_hover.png");

        // Backdrop : anchor center, size = full screen (set in UpdateLayout)
        m_Backdrop = new UIPanel(nullptr, { 0,0 }, { 0,0 }, UIAnchor::Center,
            sf::Color(0, 0, 0, 255));
        m_Backdrop->SetBlocksMouse(true);
        AddChild(m_Backdrop);

        // Dialog box centered
        m_DialogPanel = new UIPanel(nullptr, { 0,0 }, { 420,360 }, UIAnchor::Center,
            sf::Color(30, 30, 35, 235));
        AddChild(m_DialogPanel);

        // Title centered above
        m_TitleLabel = new UILabel("Paused", { 0, -130 }, UIAnchor::Center,
            m_Font, 40, { 236,222,190 });
        AddChild(m_TitleLabel);

        const sf::Vector2f BTN_SZ{ 280, 50 };

        // Buttons stacked vertically, all centered
        m_ResumeButton = new UIButton({ 0, -50 }, BTN_SZ, UIAnchor::Center,
            m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);

        m_SettingsButton = new UIButton({ 0, 10 }, BTN_SZ, UIAnchor::Center,
            m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);

        m_MainMenuButton = new UIButton({ 0, 70 }, BTN_SZ, UIAnchor::Center,
            m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);

        m_QuitButton = new UIButton({ 0, 130 }, BTN_SZ, UIAnchor::Center,
            m_WarnTex, m_WarnHoverTex, m_WarnTex);

        m_ResumeButton->SetCallback([this] { UIManager::Instance().RequestNewRoot<InGameUI>(); });
        m_SettingsButton->SetCallback([this] { if (m_OnSettings) m_OnSettings(); });
        m_MainMenuButton->SetCallback([this] { GameManager::Instance().RequestMainMenu(); });
        m_QuitButton->SetCallback([this] { GameManager::Instance().Exit(); });

        auto lbl = [&](const std::string& t, UIButton* b) {
            b->AddChild(new UILabel(t, { 0,0 }, UIAnchor::Center,
                m_Font, 22, { 243,231,207 }));
            };
        lbl("Resume", m_ResumeButton);
        lbl("Settings", m_SettingsButton);
        lbl("Main Menu", m_MainMenuButton);
        lbl("Quit Game", m_QuitButton);

        AddChild(m_ResumeButton);
        AddChild(m_SettingsButton);
        AddChild(m_MainMenuButton);
        AddChild(m_QuitButton);

        // Music player stays top-right
        m_MusicPlayer = new UIMusicPlayer({ 16,16 }, { 280,90 }, UIAnchor::BottomRight);
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

        UIGroup::UpdateLayout(view);
    }

    void PauseMenu::Draw(sf::RenderTexture& canvas)
    {
        // Don't clear — the backdrop UIPanel handles the dimming effect.
        UIGroup::Draw(canvas);
    }
}
