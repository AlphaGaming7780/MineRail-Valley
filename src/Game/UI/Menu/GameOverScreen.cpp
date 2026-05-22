#include <Game/UI/Menus/GameOverScreen.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    GameOverScreen::GameOverScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button.png");
        m_BtnHoverTex   = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");
        m_HeaderTex     = tx.Load("UI\\Statistics\\header.png");
        m_WarnTex       = tx.Load("UI\\Buttons\\warning_normal.png");
        m_WarnHoverTex  = tx.Load("UI\\Buttons\\warning_hover.png");

        m_Header = new UIImage(m_HeaderTex, { 0, 40 }, { 480, 60 }, UIAnchor::TopLeft,
                               sf::Color(220, 120, 120, 255));
        AddChild(m_Header);

        m_TitleLabel = new UILabel("GAME OVER", { 0, 60 }, UIAnchor::TopLeft,
                                   m_Font, 48, { 240, 200, 200 });
        AddChild(m_TitleLabel);

        m_ReasonLabel = new UILabel("", { 0, 160 }, UIAnchor::TopLeft,
                                    m_Font, 20, { 207, 182, 151 });
        AddChild(m_ReasonLabel);

        const sf::Vector2f BTN_SZ{ 220, 50 };
        m_RetryButton = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::BottomLeft,
                                     m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_MenuButton  = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::BottomLeft,
                                     m_WarnTex, m_WarnHoverTex, m_WarnTex);

        m_RetryButton->SetCallback([this] { if (m_OnRetry)    m_OnRetry();    });
        m_MenuButton ->SetCallback([this] { if (m_OnMainMenu) m_OnMainMenu(); });

        m_RetryButton->AddChild(new UILabel("Retry",     { 0, 0 }, UIAnchor::Center, m_Font, 22, { 243, 231, 207 }));
        m_MenuButton ->AddChild(new UILabel("Main Menu", { 0, 0 }, UIAnchor::Center, m_Font, 22, { 243, 231, 207 }));

        AddChild(m_RetryButton);
        AddChild(m_MenuButton);
    }

    GameOverScreen::~GameOverScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        tx.Unload("UI\\Statistics\\header.png");
        tx.Unload("UI\\Buttons\\warning_normal.png");
        tx.Unload("UI\\Buttons\\warning_hover.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void GameOverScreen::SetReason(const std::string& reason)
    {
        if (m_ReasonLabel) m_ReasonLabel->SetText(reason);
    }

    void GameOverScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        const float cx = m_Size.x * 0.5f;
        if (m_Header)       m_Header      ->SetRelativePosition({ cx - 240, 40 });
        if (m_TitleLabel)   m_TitleLabel  ->SetRelativePosition({ cx - 150, 60 });
        if (m_ReasonLabel)  m_ReasonLabel ->SetRelativePosition({ cx - 200, 160 });

        const float btnW = 220;
        if (m_RetryButton) m_RetryButton->SetRelativePosition({ cx - btnW * 1.1f, 80 });
        if (m_MenuButton)  m_MenuButton ->SetRelativePosition({ cx + btnW * 0.1f, 80 });

        UIGroup::UpdateLayout(view);
    }

    void GameOverScreen::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear({ 36, 22, 22 });
        UIGroup::Draw(canvas);
    }
}
