#pragma once
#include <Game/UI/UIWidget.hpp>
#include <Game/AssetDatabase.h>

namespace Game
{
    class UIProgressBar : public UIWidget
    {
    public:
        UIProgressBar(
            sf::Texture* frameTex,
            sf::Texture* barTex,
            sf::Texture* endCapTex,
            sf::Vector2f position = { 0,0 },
            sf::Vector2f size = { 100,20 },
            UIAnchor anchor = UIAnchor::TopLeft
        );

        ~UIProgressBar();

        void SetValue(float v)
        {
            m_Value = std::clamp(v, 0.f, 1.f);
            ReDraw();
        }

        float GetValue() const { return m_Value; }

        virtual void UpdateLayout(const sf::View& view) override;

        virtual void Draw(sf::RenderTexture& canvas) override;

    private:
        float m_Value = 0.f;

        sf::Texture* m_FrameTex = nullptr;
        sf::Texture* m_BarTex = nullptr;
        sf::Texture* m_EndCapTex = nullptr;

        sf::Sprite* m_FrameSprite;
        sf::Sprite* m_BarSprite;
        sf::Sprite* m_EndCapSprite;

    };
}

