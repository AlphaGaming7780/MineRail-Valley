#include <Game/UI/Widgets/UIProgressBar.hpp>
#include <Game/AssetDatabase.h>
#include <SFML/Graphics.hpp>
#include <Game/Utils/Vector2.hpp>

#include <Game/UI/UIManager.hpp>

namespace Game
{
    UIProgressBar::UIProgressBar(
        sf::Texture* frameTex,
        sf::Texture* barTex,
        sf::Texture* endCapTex,
        sf::Vector2f position, 
        sf::Vector2f size, 
        UIAnchor anchor
    ) : UIWidget(position, size, anchor)
    {
        m_FrameTex = frameTex;
        m_BarTex = barTex;
        m_EndCapTex = endCapTex;
        m_FrameSprite = new sf::Sprite(*m_FrameTex);
        m_BarSprite = new sf::Sprite(*m_BarTex);
        m_EndCapSprite = new sf::Sprite(*m_EndCapTex);
    }

    UIProgressBar::~UIProgressBar()
    {
        delete m_FrameSprite;
        delete m_BarSprite;
        delete m_EndCapSprite;
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
}