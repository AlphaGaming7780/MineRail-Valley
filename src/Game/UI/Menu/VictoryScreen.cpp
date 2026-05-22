#include <Game/UI/Menus/VictoryScreen.hpp>
#include <Game/AssetDatabase.h>
#include <Game/AudioManager.hpp>

namespace Game
{
    VictoryScreen::VictoryScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        // Play the celebratory jingle once on screen entry.
        AudioManager::Instance().PlaySound("Sounds\\Win sound.wav", 0.9f);

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button.png");
        m_BtnHoverTex   = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");
        m_HeaderTex     = tx.Load("UI\\Statistics\\header.png");
        m_CheckIconTex  = tx.Load("UI\\Icons\\check.png");

        m_Header = new UIImage(m_HeaderTex, { 0, 40 }, { 480, 60 }, UIAnchor::TopLeft);
        AddChild(m_Header);

        m_CheckIcon = new UIImage(m_CheckIconTex, { 0, 50 }, { 48, 48 }, UIAnchor::TopLeft,
                                  sf::Color(120, 220, 130, 255));
        AddChild(m_CheckIcon);

        m_TitleLabel = new UILabel("VICTORY!", { 0, 60 }, UIAnchor::TopLeft,
                                   m_Font, 48, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        const sf::Vector2f BTN_SZ{ 220, 50 };
        m_ReplayButton    = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::BottomLeft,
                                         m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_NextLevelButton = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::BottomLeft,
                                         m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_MainMenuButton  = new UIButton({ 0, 80 }, BTN_SZ, UIAnchor::BottomLeft,
                                         m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);

        m_ReplayButton   ->SetCallback([this] { if (m_OnReplay)    m_OnReplay();    });
        m_NextLevelButton->SetCallback([this] { if (m_OnNextLevel) m_OnNextLevel(); });
        m_MainMenuButton ->SetCallback([this] { if (m_OnMainMenu)  m_OnMainMenu();  });

        m_ReplayButton   ->AddChild(new UILabel("Replay",     { 0, 0 }, UIAnchor::Center, m_Font, 22, { 243, 231, 207 }));
        m_NextLevelButton->AddChild(new UILabel("Next Level", { 0, 0 }, UIAnchor::Center, m_Font, 22, { 243, 231, 207 }));
        m_MainMenuButton ->AddChild(new UILabel("Main Menu",  { 0, 0 }, UIAnchor::Center, m_Font, 22, { 243, 231, 207 }));

        AddChild(m_ReplayButton);
        AddChild(m_NextLevelButton);
        AddChild(m_MainMenuButton);
    }

    VictoryScreen::~VictoryScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        tx.Unload("UI\\Statistics\\header.png");
        tx.Unload("UI\\Icons\\check.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void VictoryScreen::SetStats(const std::vector<StatLine>& stats)
    {
        for (auto* l : m_StatLabels) { RemoveChild(l); delete l; }
        m_StatLabels.clear();

        float y = 200;
        for (const auto& s : stats)
        {
            std::string line = s.label + " : " + s.value;
            auto* lbl = new UILabel(line, { 0, y }, UIAnchor::TopLeft,
                                    m_Font, 22, { 207, 182, 151 });
            AddChild(lbl);
            m_StatLabels.push_back(lbl);
            y += 32;
        }
    }

    void VictoryScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        const float cx = m_Size.x * 0.5f;
        if (m_Header)      m_Header     ->SetRelativePosition({ cx - 240, 40 });
        if (m_CheckIcon)   m_CheckIcon  ->SetRelativePosition({ cx - 200, 50 });
        if (m_TitleLabel)  m_TitleLabel ->SetRelativePosition({ cx - 130, 60 });

        // Center stats horizontally
        float y = 200;
        for (auto* lbl : m_StatLabels) {
            lbl->SetRelativePosition({ cx - 100, y });
            y += 32;
        }

        // Three buttons in a row at the bottom
        const float btnW = 220;
        if (m_ReplayButton)    m_ReplayButton   ->SetRelativePosition({ cx - btnW * 1.6f, 80 });
        if (m_NextLevelButton) m_NextLevelButton->SetRelativePosition({ cx - btnW * 0.5f, 80 });
        if (m_MainMenuButton)  m_MainMenuButton ->SetRelativePosition({ cx + btnW * 0.6f, 80 });

        UIGroup::UpdateLayout(view);
    }

    void VictoryScreen::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear({ 25, 36, 28 });
        UIGroup::Draw(canvas);
    }
}
