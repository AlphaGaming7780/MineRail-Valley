#pragma once
#include <Game/UI.hpp>

namespace Game
{
    class SettingsScreen : public UIGroup
    {
    public:
        SettingsScreen();
        virtual ~SettingsScreen();

        void UpdateLayout(const sf::View& view) override;
        void Draw(sf::RenderTexture& canvas) override;
        
        bool HitTest(const sf::Vector2f& mousePos) const override { return true; }

    private:
        sf::Font*    m_Font          = nullptr;
        sf::Texture* m_BtnTex        = nullptr;
        sf::Texture* m_BtnHoverTex   = nullptr;
        sf::Texture* m_BtnClickedTex = nullptr;
        sf::Texture* m_TrackTex      = nullptr;
        sf::Texture* m_HandleTex     = nullptr;
        sf::Texture* m_CbOffTex      = nullptr;
        sf::Texture* m_CbOffHoverTex = nullptr;
        sf::Texture* m_CbOnTex       = nullptr;
        sf::Texture* m_CbOnHoverTex  = nullptr;
        sf::Texture* m_PanelTex      = nullptr;

        UILabel*  m_TitleLabel = nullptr;

        UILabel*  m_MasterLabel = nullptr;  UISlider* m_MasterSlider = nullptr;  UILabel* m_MasterValueLabel = nullptr;
        UILabel*  m_SoundLabel  = nullptr;  UISlider* m_SoundSlider  = nullptr;  UILabel* m_SoundValueLabel  = nullptr;
        UILabel*  m_MusicLabel  = nullptr;  UISlider* m_MusicSlider  = nullptr;  UILabel* m_MusicValueLabel  = nullptr;

        UILabel*  m_FullscreenLabel = nullptr;  UIToggle* m_FullscreenToggle = nullptr;
        UILabel*  m_VSyncLabel      = nullptr;  UIToggle* m_VSyncToggle      = nullptr;

        UIButton* m_BackButton  = nullptr;
    };
}
