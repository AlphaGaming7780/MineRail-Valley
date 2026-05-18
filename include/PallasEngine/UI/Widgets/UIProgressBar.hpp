#pragma once
#include <PallasEngine/UI/UIWidget.hpp>
#include <PallasEngine/AssetDatabase/AssetTypes/TextureAsset.hpp>

namespace pallas
{
    class UIProgressBar : public UIWidget
    {
    public:
        UIProgressBar(
            sf::Vector2f position = { 0,0 },
            sf::Vector2f size = { 100,20 },
            UIAnchor anchor = UIAnchor::TopLeft,
            TextureAsset* frameTex = nullptr,
            TextureAsset* barTex = nullptr,
            TextureAsset* endCapTex = nullptr
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

        TextureAsset* m_FrameTexAsset = nullptr;
        TextureAsset* m_BarTexAsset = nullptr;
        TextureAsset* m_EndCapTexAsset = nullptr;

        sf::Texture* m_FrameTex = nullptr;
        sf::Texture* m_BarTex = nullptr;
        sf::Texture* m_EndCapTex = nullptr;

        sf::Sprite* m_FrameSprite;
        sf::Sprite* m_BarSprite;
        sf::Sprite* m_EndCapSprite;

        sf::Texture* LoadTexture(TextureAsset** asset, const char* fallbackPath);

    };
}

