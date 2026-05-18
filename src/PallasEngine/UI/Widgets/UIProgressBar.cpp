#include <PallasEngine/UI/Widgets/UIProgressBar.hpp>
#include <PallasEngine/AssetDatabase/AssetDatabase.hpp>
#include <SFML/Graphics.hpp>
#include <PallasEngine/Utils/Vector2.hpp>

#include <PallasEngine/UI/UIManager.hpp>

namespace pallas
{
	UIProgressBar::UIProgressBar(
        sf::Vector2f position, 
        sf::Vector2f size, 
        UIAnchor anchor, 
        TextureAsset* frameTex, 
        TextureAsset* barTex, 
        TextureAsset* endCapTex
    )
        : UIWidget(position, size, anchor),
        m_FrameTexAsset(frameTex),
        m_BarTexAsset(barTex),
        m_EndCapTexAsset(endCapTex)
    {
    
        m_FrameTex = LoadTexture(&m_FrameTexAsset, "UI\\Loading_bar_frame.png");
        m_BarTex = LoadTexture(&m_BarTexAsset, "UI\\Loading_bar_inner.png");
        m_EndCapTex = LoadTexture(&m_EndCapTexAsset, "UI\\Progress_pivot.png");
    
        m_FrameSprite = new sf::Sprite(*m_FrameTex);
        m_BarSprite = new sf::Sprite(*m_BarTex);
        m_EndCapSprite = new sf::Sprite(*m_EndCapTex);
    }

    UIProgressBar::~UIProgressBar()
    {
        delete m_FrameSprite, m_BarSprite, m_EndCapSprite;
        m_FrameTexAsset->Unload();
        m_BarTexAsset->Unload();
        m_EndCapTexAsset->Unload();
    }

    void UIProgressBar::UpdateLayout(const sf::View& view)
    {
        UIWidget::UpdateLayout(view);
        m_FrameSprite->setPosition(m_AbsPosition);

        auto frameScale = m_Size / (sf::Vector2f)m_FrameTex->getSize();
        m_FrameSprite->setScale(frameScale);
        m_BarSprite->setScale(frameScale);
        m_EndCapSprite->setScale(frameScale);
        
        float totalBarWith = (float)(m_BarTex->getSize().x) * frameScale.x;
        float filledWidth = totalBarWith * m_Value;

        m_BarSprite->setPosition(m_AbsPosition + (m_FrameSprite->getGlobalBounds().size - (sf::Vector2f)m_BarTex->getSize() * frameScale) / 2.f );
        m_BarSprite->setTextureRect( { 
            { 0, 0 }, 
            { static_cast<int>(m_BarTex->getSize().x * m_Value), static_cast<int>(m_BarTex->getSize().y)}
        });

        float endCapX = m_BarSprite->getGlobalBounds().position.x + filledWidth - m_EndCapSprite->getGlobalBounds().size.x / 2.f;
        float endCapY = m_BarSprite->getGlobalBounds().position.y + (m_BarSprite->getGlobalBounds().size.y - m_EndCapSprite->getGlobalBounds().size.y) / 2.f;
        m_EndCapSprite->setPosition({endCapX, endCapY});
    }

    void UIProgressBar::Draw(sf::RenderTexture& canvas)
    {
        canvas.draw(*m_FrameSprite);
        canvas.draw(*m_BarSprite);

        if (m_Value > 0.f && m_Value < 1.f)
            canvas.draw(*m_EndCapSprite);

        UIWidget::Draw(canvas);
    }


    sf::Texture* UIProgressBar::LoadTexture(TextureAsset** asset, const char* fallbackPath)
    {
        if (*asset == nullptr)
        {
            if (!AssetDatabase::Instance().TryGetAsset(fallbackPath, *asset))
            {
                (*asset) = TextureAsset::GetDefault();
            }
        }

        return (*asset)->LoadObject();
    }
}