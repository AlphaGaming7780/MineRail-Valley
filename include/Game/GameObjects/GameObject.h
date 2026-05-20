#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include <Game/AssetDatabase/TextureDatabase.h>
#include <Game/AssetDatabase/AssetType/ObjectData.h>
#include <Game/Rendering/RenderLayer.h>

namespace Game
{
	class GameObject
	{
		friend class World;
	public:

		virtual ~GameObject() = default;

		std::string m_Texture = "Textures\\DEFAULT.png";
		sf::Vector2f m_Size = { 100.f, 100.f };

		bool m_Animated = false;
		int m_AnimFramesX = 1;
		int m_AnimFramesY = 1;
		float m_AnimCurrentTime = 0;
		float m_AnimDuration = 0.f;

		sf::Sprite* m_Sprite = nullptr;
		int zIndex = 0;
		RenderLayer m_RenderLayer = RenderLayer::NONE;

		bool m_Enabled = false;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		virtual void OnCreate() {};
		virtual void Update();
		virtual void OnDestroy();

		void SetSize(const sf::Vector2f& size);
		sf::Vector2f GetSize() const { return m_Size; }

		void Move(const sf::Vector2f& move);

		void SetPosition(const sf::Vector2f& position);
		sf::Vector2f GetPosition() const;

		sf::FloatRect GetGlobalBounds() const;
		sf::FloatRect GetLocalBounds() const;


	protected:
		GameObject();
		GameObject(const ObjectData& data);

		sf::Vector2f GetActualScale();
	};
}