#include <Game/UI/Menus/LevelSelectScreen.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    LevelSelectScreen::LevelSelectScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();

        m_Font           = fn.Load("Fonts\\Minecraft.ttf");
        m_BtnTex         = tx.Load("UI\\Button.png");
        m_BtnHoverTex    = tx.Load("UI\\Button_hover.png");
        m_BtnClickedTex  = tx.Load("UI\\Button_clicked.png");
        m_JoinTex        = tx.Load("UI\\LevelSelect\\join_normal.png");
        m_JoinHoverTex   = tx.Load("UI\\LevelSelect\\join_hover.png");
        m_MarkedTex      = tx.Load("UI\\LevelSelect\\marked_normal.png");
        m_MarkedHoverTex = tx.Load("UI\\LevelSelect\\marked_hover.png");
        m_WarnTex        = tx.Load("UI\\LevelSelect\\warning_normal.png");
        m_WarnHoverTex   = tx.Load("UI\\LevelSelect\\warning_hover.png");
        m_PanelTex       = tx.Load("UI\\Panels\\popup_bg.png");

        m_TitleLabel = new UILabel("Select Level", { 24, 16 }, UIAnchor::TopLeft,
                                   m_Font, 40, { 236, 222, 190 });
        AddChild(m_TitleLabel);

        m_BackButton = new UIButton({ 24, 16 }, { 120, 48 }, UIAnchor::BottomLeft,
                                    m_BtnTex, m_BtnHoverTex, m_BtnClickedTex);
        m_BackButton->SetCallback([this] { if (m_OnBack) m_OnBack(); });
        m_BackButton->AddChild(new UILabel("Back", { 0, 0 }, UIAnchor::Center,
                                           m_Font, 22, { 243, 231, 207 }));
        AddChild(m_BackButton);

        // Default level set — replace by reading the Maps directory or by a
        // call to SetLevels() right after constructing the screen.
        SetLevels({
            { "Maps\\Plain.json",            "Plain",            "Plain",    1, false },
            { "Maps\\ForestValley.json",     "Forest Valley",    "Forest",   2, false },
            { "Maps\\FrozenHighlands.json",  "Frozen Highlands", "Frozen",   3, false },
            { "Maps\\DesertCanyon.json",     "Desert Canyon",    "Desert",   3, false },
            { "Maps\\VolcanicWastes.json",   "Volcanic Wastes",  "Volcanic", 4, false },
            { "Maps\\NeonCity.json",         "Neon City",        "Neon",     4, false },
            { "Maps\\DEFAULT.json",          "Default Sandbox",  "Default",  1, false },
        });
    }

    LevelSelectScreen::~LevelSelectScreen()
    {
        TextureDatabase& tx = TextureDatabase::Instance();
        FontDatabase&    fn = FontDatabase::Instance();
        tx.Unload("UI\\Button.png");
        tx.Unload("UI\\Button_hover.png");
        tx.Unload("UI\\Button_clicked.png");
        tx.Unload("UI\\LevelSelect\\join_normal.png");
        tx.Unload("UI\\LevelSelect\\join_hover.png");
        tx.Unload("UI\\LevelSelect\\marked_normal.png");
        tx.Unload("UI\\LevelSelect\\marked_hover.png");
        tx.Unload("UI\\LevelSelect\\warning_normal.png");
        tx.Unload("UI\\LevelSelect\\warning_hover.png");
        tx.Unload("UI\\Panels\\popup_bg.png");
        fn.Unload("Fonts\\Minecraft.ttf");
    }

    void LevelSelectScreen::SetLevels(const std::vector<LevelInfo>& levels)
    {
        m_Levels = levels;
        Rebuild();
    }

    void LevelSelectScreen::Rebuild()
    {
        // Drop previous cards
        for (auto* c : m_Cards) { RemoveChild(c); delete c; }
        m_Cards.clear();

        // Layout in a 3-column grid; positions are absolute relative to the
        // screen origin and finalized by UpdateLayout (which centers the grid).
        const sf::Vector2f CARD_SZ{ 280, 160 };
        const float GAP_X = 24, GAP_Y = 24;
        const int   COLS  = 3;

        for (size_t i = 0; i < m_Levels.size(); ++i)
        {
            const auto& L = m_Levels[i];
            int row = int(i) / COLS;
            int col = int(i) % COLS;
            sf::Vector2f cardPos = {
                col * (CARD_SZ.x + GAP_X) + 24,
                row * (CARD_SZ.y + GAP_Y) + 96
            };

            auto* card = new UIPanel(m_PanelTex, cardPos, CARD_SZ, UIAnchor::TopLeft);

            // Title
            card->AddChild(new UILabel(L.title, { 16, 12 }, UIAnchor::TopLeft,
                                        m_Font, 22, { 236, 222, 190 }));
            // Biome + difficulty
            std::string sub = L.biome + "   Diff:";
            for (int d = 0; d < L.difficulty; ++d) sub += "*";
            card->AddChild(new UILabel(sub, { 16, 44 }, UIAnchor::TopLeft,
                                        m_Font, 14, { 207, 182, 151 }));

            // Play button uses join_* if unlocked, warning_* if locked
            sf::Texture* nrm = L.locked ? m_WarnTex      : m_JoinTex;
            sf::Texture* hov = L.locked ? m_WarnHoverTex : m_JoinHoverTex;
            auto* play = new UIButton({ CARD_SZ.x - 90, CARD_SZ.y - 50 },
                                      { 70, 36 }, UIAnchor::TopLeft, nrm, hov, nrm);
            std::string mapPath = L.mapPath;
            bool locked = L.locked;
            play->SetCallback([this, mapPath, locked] {
                if (locked) return;
                if (m_OnPlay) m_OnPlay(mapPath);
            });
            card->AddChild(play);

            AddChild(card);
            m_Cards.push_back(card);
        }
    }

    void LevelSelectScreen::UpdateLayout(const sf::View& view)
    {
        m_Size = view.getSize();
        UIGroup::UpdateLayout(view);
    }

    void LevelSelectScreen::Draw(sf::RenderTexture& canvas)
    {
        canvas.clear({ 24, 26, 30 });
        UIGroup::Draw(canvas);
    }
}
