#include <Game/UI/Menus/CreditsScreen.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    // Edit this list to customize the credits content.
    static const char* CREDITS_LINES[] = {
        "MineRail Valley",
        "",
        "Programming",
        "Triton Sypreme",
        "Math0xk",
        "",
        "Engine",
        "PallasEngine (in-house)",
        "",
        "Libraries",
        "SFML 3 — graphics, audio, networking",
        "glaze — JSON serialization",
        "magic_enum — enum reflection",
        "libssh2, mbedtls — networking primitives",
        "",
        "Music",
        "0. Menus — Various artists",
        "1. Night Drive — Kavinsky, M83, Brian Eno, etc.",
        "2. Industrial Ambient",
        "3. Lo-Fi Journey — C418, Toby Fox, etc.",
        "4. Zen",
        "5. Neons & Rain",
        "",
        "Textures",
        "Minecraft texture pack (block & GUI sprites)",
        "",
        "Thanks for playing!",
    };

    CreditsScreen::CreditsScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font          = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex        = tx.Load("UI\\Button.png");
        m_BtnHoverTex   = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex = tx.Load("UI\\Button_clicked.png");

        m_TitleLabel = new UILabel("Credits", { 24, 16 }, UIAnchor::TopLeft,
                                   m_Font, 40, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        // Build the credit lines
        float y = 100;
        for (auto* line : CREDITS_LINES)
        {
            auto* l = new UILabel(line, { 80, y }, UIAnchor::TopLeft,
                                  m_Font, 18, { 207, 182, 151 });
            AddChild(l);
            m_Lines.push_back(l);
            y += 26;
        }

        m_BackButton = new UIButton({ 24, 16 }, { 120, 48 }, UIAnchor::BottomLeft,
                                    m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_BackButton->SetCallback([this] { if (m_OnBack) m_OnBack(); });
        m_BackButton->AddChild(new UILabel("Back", { 0, 0 }, UIAnchor::Center,
                                           m_Font, 22, { 243, 231, 207 }));
        AddChild(m_BackButton);
    }

    CreditsScreen::~CreditsScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void CreditsScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        UIGroup::UpdateLayout(view);
    }

    void CreditsScreen::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear({ 19, 20, 22 });
        UIGroup::Draw(canvas);
    }
}
