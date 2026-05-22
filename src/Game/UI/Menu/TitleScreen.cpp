#include <Game/UI/Menus/TitleScreen.hpp>
#include <Game/UI/Menus/LevelSelectScreen.hpp>
#include <Game/UI/Menus/SettingsScreen.hpp>
#include <Game/UI/Menus/CreditsScreen.hpp>
#include <Game/GameManager.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    TitleScreen::TitleScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button_large.png");           // reuse existing
        m_BtnHoverTex   = tx.Load("UI\\Button_large_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_large_clicked.png");
        m_WarnTex       = tx.Load("UI\\Buttons\\warning_normal.png");
        m_WarnHoverTex  = tx.Load("UI\\Buttons\\warning_hover.png");

        // Title (anchored top-center via a top-left offset; the screen layout
        // recenters it in UpdateLayout based on view size).
        m_TitleLabel = new UILabel("MINERAIL VALLEY",
                                   { 0, -160 }, UIAnchor::Center,
                                   m_Font, 64, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        // Buttons stack — recentered by UpdateLayout.
        const sf::Vector2f BTN_SZ{ 260, 60 };
        m_PlayButton     = new UIButton({ 0, 0 }, BTN_SZ, UIAnchor::Center,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_SettingsButton = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::Center,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_CreditsButton  = new UIButton({ 0, 160 }, BTN_SZ, UIAnchor::Center,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_QuitButton     = new UIButton({ 0, 240 }, BTN_SZ, UIAnchor::Center,
                                        m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);

        m_PlayButton->SetCallback([this]     { UIManager::Instance().RequestNewRoot<LevelSelectScreen>(); });
        m_SettingsButton->SetCallback([this] { UIManager::Instance().RequestNewRoot<SettingsScreen>(); });
        m_CreditsButton ->SetCallback([this] { UIManager::Instance().RequestNewRoot<CreditsScreen>(); });
        m_QuitButton    ->SetCallback([this] { GameManager::Instance().Exit(); });

        // Labels living inside the buttons
        auto addBtnLabel = [&](UIButton* btn, const std::string& text) {
            auto* l = new UILabel(text, { 0, 0 }, UIAnchor::Center, m_Font, 26, { 243, 231, 207 });
            btn->AddChild(l);
        };
        addBtnLabel(m_PlayButton,     "Play");
        addBtnLabel(m_SettingsButton, "Settings");
        addBtnLabel(m_CreditsButton,  "Credits");
        addBtnLabel(m_QuitButton,     "Quit");

        AddChild(m_PlayButton);
        AddChild(m_SettingsButton);
        AddChild(m_CreditsButton);
        AddChild(m_QuitButton);

        // Version label, bottom-left
        m_VersionLabel = new UILabel("v0.1", { 12, 12 }, UIAnchor::BottomLeft,
                                     m_Font, 14, { 207, 182, 151 });
        AddChild(m_VersionLabel);
    }

    TitleScreen::~TitleScreen()
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

    void TitleScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();

        // Re-center buttons horizontally given the current window size.
        //const float cx = m_Size.x * 0.5f;
        //const float btnW = 260.f;
        //const float titleW = 480.f; // approximate; SFML doesn't expose text width without measure.

        //if (m_TitleLabel)    m_TitleLabel   ->SetRelativePosition({ cx - titleW * 0.5f, 80 });
        //if (m_PlayButton)    m_PlayButton   ->SetRelativePosition({ cx - btnW * 0.5f, 240 });
        //if (m_SettingsButton)m_SettingsButton->SetRelativePosition({ cx - btnW * 0.5f, 310 });
        //if (m_CreditsButton) m_CreditsButton->SetRelativePosition({ cx - btnW * 0.5f, 380 });
        //if (m_QuitButton)    m_QuitButton   ->SetRelativePosition({ cx - btnW * 0.5f, 460 });

        UIGroup::UpdateLayout(view);
    }

    void TitleScreen::Draw(sf::RenderTexture& canvas)
    {
        // Solid dark background. Replace with a parallax screenshot/logo later.
        canvas.clear({ 19, 20, 22 });
        UIGroup::Draw(canvas);
    }
}
