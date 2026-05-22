#pragma once
#include <SFML/Graphics.hpp>
#include <Game/UI/UIWidget.hpp>

namespace Game
{
    // UIPanel: decorative background rectangle used to group widgets visually.
    //
    // Two rendering modes:
    //   1) Textured  — pass a texture (e.g. UI/Panels/popup_bg.png). The texture
    //                  is stretched to fill m_Size. For 9-slice support, see the
    //                  TODO at the bottom of this file.
    //   2) Solid color — leave the texture nullptr and the panel will be filled
    //                    with m_Color (useful for menus with no texture yet).
    //
    // UIPanel is a transparent-to-mouse element: HitTest returns false so it
    // doesn't intercept clicks meant for child widgets above it. If you need a
    // panel that *blocks* clicks (e.g. modal dialog backdrop), set
    // m_BlocksMouse = true via SetBlocksMouse().
    class UIPanel : public UIWidget
    {
    public:
        UIPanel(
            sf::Texture* background = nullptr,
            sf::Vector2f position = { 0, 0 },
            sf::Vector2f size = { 0, 0 },
            UIAnchor anchor = UIAnchor::TopLeft,
            sf::Color color = sf::Color(0, 0, 0, 160)
        );

        virtual ~UIPanel();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;

        void SetTexture(sf::Texture* texture);
        void SetColor(const sf::Color& color);

        // When true, the panel HitTests as a regular rect (blocks mouse events
        // from reaching widgets below it in the same screen). Default = false.
        void SetBlocksMouse(bool b) { m_BlocksMouse = b; }
        bool HitTest(const sf::Vector2f& mousePos) const override;

    private:
        sf::Texture*       m_Texture = nullptr;
        sf::Sprite*        m_Sprite  = nullptr;
        sf::RectangleShape m_Rect;
        sf::Color          m_Color;
        bool               m_BlocksMouse = false;
    };
}

// TODO (optional improvement, not blocking for first wiring):
//   Implement 9-slice rendering so the corners stay pixel-perfect when the panel
//   is stretched (the Minecraft-style textures from .IDK/gui/sprites/popup are
//   designed for 9-slicing). For now, simple stretch is fine.
