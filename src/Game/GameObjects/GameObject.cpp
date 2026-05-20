#include <Game/GameObjects/GameObject.h>
#include <Game/Utils/AnimatedTextureUtils.h>
#include <Game/Utils/Time.hpp>

namespace Game
{
    void GameObject::Update()
    {
        if (m_Animated) AnimatedTextureUtils::UpdateFrame(*this, Time::GetDeltaTime().asSeconds());
    }

    void GameObject::OnDestroy()
	{
        delete m_Sprite;
        TextureDatabase::Instance().Unload(m_Texture);
	}

	void GameObject::SetSize(const sf::Vector2f& size)
	{
        m_Size = size;
        m_Sprite->scale(GetActualScale());
	}

	void GameObject::Move(const sf::Vector2f& offset)
	{
		m_Sprite->move(offset);
	}

    void GameObject::SetPosition(const sf::Vector2f& position)
    {
        m_Sprite->setPosition(position);
    }

    sf::Vector2f GameObject::GetPosition() const
    {
        return m_Sprite->getPosition();
    }

    sf::FloatRect GameObject::GetGlobalBounds() const
    {
        return m_Sprite->getGlobalBounds();
    }

    sf::FloatRect GameObject::GetLocalBounds() const
    {
        return m_Sprite->getLocalBounds();
    }

    void GameObject::SetColor(const sf::Color& color)
    {
        m_Sprite->setColor(color);
    }

    void GameObject::ResetColor()
    {
        m_Sprite->setColor(m_Color);
    }

    void GameObject::SetScale(sf::Vector2f scale)
    {
        m_Scale = scale;
        m_Sprite->setScale(GetActualScale());
    }

    sf::Vector2f GameObject::GetScale() const
    {
        return m_Scale;
    }

    GameObject::GameObject()
    {
        m_Sprite = new sf::Sprite(*TextureDatabase::Instance().Load(m_Texture));
    }

    GameObject::GameObject(const ObjectData& data)
	{
        m_Texture = data.m_Texture;
        m_Size = data.m_Size;
        m_Animated = data.m_Animated;
        m_AnimDuration = data.m_AnimDuration;
        m_AnimFramesX = data.m_AnimFramesX;
        m_AnimFramesY = data.m_AnimFramesY;
        m_Color = data.m_Color;

        m_Sprite = new sf::Sprite(*TextureDatabase::Instance().Load(m_Texture));
        if (m_Animated) AnimatedTextureUtils::UpdateFrame(*this, Time::GetDeltaTime().asSeconds());
        m_Sprite->setOrigin(m_Sprite->getLocalBounds().size / 2.f);
        m_Sprite->scale(GetActualScale());
        m_Sprite->setColor(m_Color);
	}

	sf::Vector2f GameObject::GetActualScale()
    {
        // Taille brute de la texture ou du frame animé
        sf::Vector2f baseSize = static_cast<sf::Vector2f>(m_Sprite->getTexture().getSize());

        if (m_Animated)
        {
            baseSize = sf::Vector2f(
                baseSize.x / m_AnimFramesX,
                baseSize.y / m_AnimFramesY
            );
        }

        // Éviter division par zéro
        if (baseSize.x == 0.f || baseSize.y == 0.f)
            return { 0.f, 0.f };

        // Scale absolu (size / textureSize)
        sf::Vector2f absoluteScale = {
            m_Size.x / baseSize.x,
            m_Size.y / baseSize.y
        };

        // Scale final = scale absolu * scale SFML
        return absoluteScale * m_Scale;
    }
}