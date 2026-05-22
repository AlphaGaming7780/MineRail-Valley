#pragma once
#include <SFML/Graphics.hpp>
#include <Game/UI/UIWidget.hpp>
#include <functional>

namespace Game
{
    // UIToggle: simple on/off checkbox-style widget.
    //
    // Four textures are used to render each state combination:
    //   m_TexOffNormal / m_TexOffHover : when m_Checked == false
    //   m_TexOnNormal  / m_TexOnHover  : when m_Checked == true
    //
    // The textures from assets/UI/Checkboxes/ are pre-organised for that exact
    // mapping (off_normal, off_hover, on_normal, on_hover).
    class UIToggle : public UIWidget
    {
    public:
        UIToggle(
            sf::Vector2f position = { 0, 0 },
            sf::Vector2f size = { 0, 0 },
            UIAnchor anchor = UIAnchor::TopLeft,
            sf::Texture* offNormal = nullptr,
            sf::Texture* offHover  = nullptr,
            sf::Texture* onNormal  = nullptr,
            sf::Texture* onHover   = nullptr
        );

        virtual ~UIToggle();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void OnClick()      override;

        // State
        bool IsChecked() const { return m_Checked; }
        void SetChecked(bool checked);

        // Callback signature receives the new state right after toggling.
        void SetCallback(std::function<void(bool)> cb) { m_Callback = std::move(cb); }

    private:
        sf::Texture* m_TexOffNormal = nullptr;
        sf::Texture* m_TexOffHover  = nullptr;
        sf::Texture* m_TexOnNormal  = nullptr;
        sf::Texture* m_TexOnHover   = nullptr;

        sf::Sprite* m_Sprite = nullptr;

        bool m_Checked = false;
        std::function<void(bool)> m_Callback;

        void RefreshSprite();
    };
}
