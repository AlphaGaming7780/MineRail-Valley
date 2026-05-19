#pragma once
#include<Game/AssetDatabase.h>
#include<string>

namespace Game
{
	struct ObjectData
	{
		std::string m_Texture = "Textures\\DEFAULT.png";
		sf::Color m_Color = sf::Color::White;
		sf::Vector2f m_Size = { 1.f, 1.f };
		

		bool m_Animated = false;
		int m_AnimFramesX = 1;       // nombre de frames en X
		int m_AnimFramesY = 1;       // nombre de frames en 
		float m_AnimDuration = 0.f;
	};

	struct ObjectAsset
	{
		sf::Texture* m_Texture = nullptr;
		sf::Color m_Color = sf::Color::White;
		sf::Vector2f m_Size = { 1.f, 1.f };


		bool m_Animated = false;
		int m_AnimFramesX = 1;       // nombre de frames en X
		int m_AnimFramesY = 1;       // nombre de frames en 
		float m_AnimDuration = 0.f;
	};

}