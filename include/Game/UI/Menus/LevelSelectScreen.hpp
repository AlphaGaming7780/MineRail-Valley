#pragma once
#include <Game/UI.hpp>
#include <vector>

namespace Game
{
    // LevelSelectScreen — grid of level cards, one per map.
    //
    // Each card displays:
    //   - the map name (e.g. "Plain")
    //   - the biome tag (e.g. "Plain", "Frozen", ...)
    //   - the difficulty (read from MapData::Difficulty, 1..5)
    //   - a tiny color preview (for now: a flat color from biome → palette)
    //   - a [Play] button (uses LevelSelect/join* assets)
    //
    // Wiring TODO:
    //   - Build the level list by reading every Maps/*.json (or hard-code a
    //     list for now). See LevelSelectScreen::SetLevels below.
    //   - SetOnPlayLevel([](const std::string& mapPath) { LoadLevel(mapPath); })
    //   - SetOnBack    ([] { ShowTitle(); });
    class LevelSelectScreen : public UIGroup
    {
    public:
        struct LevelInfo
        {
            std::string mapPath;   // e.g. "Maps\\Plain.json"
            std::string title;     // displayed name, e.g. "Plain"
            std::string biome;     // displayed biome, e.g. "Plain"
            int         difficulty = 1;
            bool        locked     = false;  // if true, render the locked variant
        };

        LevelSelectScreen();
        virtual ~LevelSelectScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Provide the list of levels to show. Calling SetLevels rebuilds the
        // grid (deletes the previous level cards). Safe to call at any time.
        void SetLevels(const std::vector<LevelInfo>& levels);

        void SetOnPlayLevel(std::function<void(const std::string&)> cb) { m_OnPlay = std::move(cb); }
        void SetOnBack     (std::function<void()> cb)                   { m_OnBack = std::move(cb); }

    private:
        // Builds (or rebuilds) the visual cards from m_Levels.
        void Rebuild();

        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_JoinTex       = nullptr;
        sf::Texture* m_JoinHoverTex  = nullptr;
        sf::Texture* m_MarkedTex     = nullptr;
        sf::Texture* m_MarkedHoverTex= nullptr;
        sf::Texture* m_WarnTex       = nullptr;
        sf::Texture* m_WarnHoverTex  = nullptr;
        sf::Texture* m_PanelTex      = nullptr;

        UILabel*   m_TitleLabel = nullptr;
        UIButton*  m_BackButton = nullptr;

        std::vector<LevelInfo> m_Levels;
        std::vector<UIElement*> m_Cards;        // current card widgets (for cleanup)

        std::function<void(const std::string&)> m_OnPlay;
        std::function<void()>                   m_OnBack;
    };
}
