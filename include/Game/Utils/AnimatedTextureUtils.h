#pragma once

#include <SFML/Graphics.hpp>
#include <Game/GameObjects/GameObject.h>

namespace Game
{
	class AnimatedTextureUtils
	{
	public:
        static void UpdateFrame(GameObject& obj, float deltaTime)
        {

            if (obj.m_Sprite == nullptr || obj.m_AnimFramesX * obj.m_AnimFramesY <= 1)
                return;

            // Avance du temps
            obj.m_AnimCurrentTime += deltaTime;
            if (obj.m_AnimCurrentTime >= obj.m_AnimDuration)
                obj.m_AnimCurrentTime -= obj.m_AnimDuration;

            const int totalFrames = obj.m_AnimFramesX * obj.m_AnimFramesY;
            float frameDuration = obj.m_AnimDuration / totalFrames;

            int frameIndex = static_cast<int>(obj.m_AnimCurrentTime / frameDuration);

            int frameX = frameIndex % obj.m_AnimFramesX;
            int frameY = frameIndex / obj.m_AnimFramesX;

            const sf::Texture& texture = obj.m_Sprite->getTexture();
            int frameWidth = texture.getSize().x / obj.m_AnimFramesX;
            int frameHeight = texture.getSize().y / obj.m_AnimFramesY;

            obj.m_Sprite->setTextureRect(sf::IntRect({
                {
                    frameX * frameWidth,
                    frameY * frameHeight,
                },{
                    frameWidth,
                    frameHeight
                }
            }));
        }
	};
}
