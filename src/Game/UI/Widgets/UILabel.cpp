#include <Game/UI/Widgets/UILabel.hpp>
#include <Game/AssetDatabase/FontDatabase.h>

namespace Game
{
    UILabel::UILabel(
        const std::string& text,
        sf::Vector2f position,
        UIAnchor anchor,
        sf::Font* fontAsset,
        unsigned int characterSize
    )
        : UIWidget(position, {0,0}, anchor)
    {
        m_FontAsset = fontAsset;

        // Charge la police (ou fallback)
        m_Font = LoadFont(&m_FontAsset, FontAsset::DefaultPath);

        // Crée le texte SFML
        m_Text = new sf::Text(*m_Font, text, characterSize);
        m_Text->setFillColor({207, 182, 151});
    }

    UILabel::~UILabel()
    {
        delete m_Text;

        //if (m_FontAsset) Il devrait tjs y avoir un Font Asset.
        m_FontAsset->Unload();
    }

    sf::Font* UILabel::LoadFont(FontAsset** asset, const char* fallbackPath)
    {
        if (*asset == nullptr)
        {
            if (!AssetDatabase::Instance().TryGetAsset(fallbackPath, *asset))
            {
                *asset = FontAsset::GetDefault();
            }
        }

        return (*asset)->LoadObject();
    }

    void UILabel::UpdateLayout(const sf::View& view)
    {
        m_Size = m_Text->getGlobalBounds().size;
        UIWidget::UpdateLayout(view);

        // Position du texte
        auto offset = m_Text->getGlobalBounds().position - m_Text->getPosition();
        m_Text->setPosition(m_AbsPosition - offset);

        // Optionnel : scale automatique si tu veux que le texte rentre dans m_Size
        // (désactivé par défaut)
    }

    void UILabel::Draw(sf::RenderTexture& canvas)
    {
        canvas.draw(*m_Text);

        // Dessine les enfants
        UIWidget::Draw(canvas);
    }

    void UILabel::SetText(const std::string& text)
    {
        m_Text->setString(text);
        ReDraw();
    }

    void UILabel::SetColor(const sf::Color& color)
    {
        m_Text->setFillColor(color);
        ReDraw();
    }

    void UILabel::SetCharacterSize(unsigned int size)
    {
        m_Text->setCharacterSize(size);
        ReDraw();
    }
}