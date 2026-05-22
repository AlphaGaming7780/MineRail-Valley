#pragma once
#include <Game/UI.hpp>

namespace Game
{
    // VictoryScreen — shown when the player completes a level.
    //
    // The screen is intentionally minimal. It exposes setters so your game
    // code can fill in any computed values (score, time, rank) before showing.
    //
    // Wiring TODO:
    //   victory->SetStats({ {"Score", "1240"}, {"Time", "3:14"}, {"Rank", "A"} });
    //   victory->SetOnReplay   ([] { ReloadCurrentLevel(); });
    //   victory->SetOnNextLevel([] { LoadNextLevel(); });
    //   victory->SetOnMainMenu ([] { ShowTitle(); });
    class VictoryScreen : public UIGroup
    {
    public:
        struct StatLine { std::string label; std::string value; };

        VictoryScreen();
        virtual ~VictoryScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        // Fill in the stat lines shown to the player. Calling SetStats rebuilds
        // the label list, so call it once before pushing the screen.
        void SetStats(const std::vector<StatLine>& stats);

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_HeaderTex     = nullptr;
        sf::Texture* m_CheckIconTex  = nullptr;

        UIImage*  m_Header     = nullptr;
        UIImage*  m_CheckIcon  = nullptr;
        UILabel*  m_TitleLabel = nullptr;
        std::vector<UILabel*> m_StatLabels;
        UIButton* m_ReplayButton    = nullptr;
        UIButton* m_NextLevelButton = nullptr;
        UIButton* m_MainMenuButton  = nullptr;
    };
}
