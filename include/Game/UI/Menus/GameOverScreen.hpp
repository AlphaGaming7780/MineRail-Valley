#pragma once
#include <Game/UI.hpp>

namespace Game
{
    // GameOverScreen — shown when the player fails a level.
    //
    // Mirrors VictoryScreen but with a red tint and "Game Over" header.
    //
    // Wiring TODO:
    //   over->SetReason("You ran out of money");   // optional
    //   over->SetOnRetry   ([] { ReloadCurrentLevel(); });
    //   over->SetOnMainMenu([] { ShowTitle(); });
    class GameOverScreen : public UIGroup
    {
    public:
        GameOverScreen();
        virtual ~GameOverScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        void SetReason(const std::string& reason);

        void SetOnRetry   (std::function<void()> cb) { m_OnRetry    = std::move(cb); }
        void SetOnMainMenu(std::function<void()> cb) { m_OnMainMenu = std::move(cb); }

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_HeaderTex     = nullptr;
        sf::Texture* m_WarnTex       = nullptr;
        sf::Texture* m_WarnHoverTex  = nullptr;

        UIImage*  m_Header       = nullptr;
        UILabel*  m_TitleLabel   = nullptr;
        UILabel*  m_ReasonLabel  = nullptr;
        UIButton* m_RetryButton  = nullptr;
        UIButton* m_MenuButton   = nullptr;

        std::function<void()> m_OnRetry;
        std::function<void()> m_OnMainMenu;
    };
}
