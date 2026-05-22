#pragma once
#include <Game/UI.hpp>
#include <vector>

namespace Game
{
    // CreditsScreen — scrolling list of credits.
    //
    // Layout: a tall stack of UILabels arranged in a vertical column. For now
    // the scroll is manual (a top/bottom button) — we can swap in a real
    // scrollbar later by using assets/UI/Scroll/.
    //
    // Wiring TODO:
    //   credits->SetOnBack([] { ShowTitle(); });
    //
    // The contents of the credits are hard-coded in the .cpp — feel free to
    // edit the static vector there.
    class CreditsScreen : public UIGroup
    {
    public:
        CreditsScreen();
        virtual ~CreditsScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        void SetOnBack(std::function<void()> cb) { m_OnBack = std::move(cb); }

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;

        UILabel*  m_TitleLabel = nullptr;
        UIButton* m_BackButton = nullptr;

        std::vector<UILabel*> m_Lines;
        std::function<void()> m_OnBack;
    };
}
