#pragma once
#include <PallasEngine/ECS/Components/Animated.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pallas
{
    struct AnimationFrame
    {
        bool valid = false;
        int frameX = 0;
        int frameY = 0;
        int frameWidth = 0;
        int frameHeight = 0;
    };

    class AnimatedTextureUtils
    {
    public:
        static AnimationFrame ComputeFrame(Animated& anim, sf::Texture* texture, float deltaTime)
        {
            AnimationFrame result;

            if (!texture || anim.m_FramesX * anim.m_FramesY <= 1)
                return result;

            // Avance du temps
            anim.m_CurrentTime += deltaTime;
            if (anim.m_CurrentTime >= anim.m_Duration)
                anim.m_CurrentTime -= anim.m_Duration;

            const int totalFrames = anim.m_FramesX * anim.m_FramesY;
            float frameDuration = anim.m_Duration / totalFrames;

            int frameIndex = static_cast<int>(anim.m_CurrentTime / frameDuration);

            result.frameX = frameIndex % anim.m_FramesX;
            result.frameY = frameIndex / anim.m_FramesX;

            result.frameWidth = texture->getSize().x / anim.m_FramesX;
            result.frameHeight = texture->getSize().y / anim.m_FramesY;

            result.valid = true;
            return result;
        }
    };
}