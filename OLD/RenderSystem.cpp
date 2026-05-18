#include <PallasEngine/ECS/Systems/RenderSystem.hpp>
#include <PallasEngine/ECS/Components/Transform.hpp>
#include <PallasEngine/ECS/Components/Renderer.hpp>


namespace pallas
{
	void RenderSystem::Update()
    {
        if (!m_Window) return;

        auto view = m_Registry->view<Transform, Renderer>();

        // On copie les entités dans un buffer pour pouvoir trier
        m_DrawList.clear();
        m_DrawList.reserve(view.size_hint());

        for (auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            auto& renderer = view.get<Renderer>(entity);

            if (!renderer.visible)
                continue;

            DrawItem item = { entity, renderer.m_zIndex };
            m_DrawList.push_back(item);
        }

        // Tri par zIndex
        std::sort(m_DrawList.begin(), m_DrawList.end(),
            [](const DrawItem& a, const DrawItem& b)
            {
                return a.zIndex < b.zIndex;
            });

        // Dessin
        for (auto& item : m_DrawList)
        {
            auto& transform = m_Registry->get<Transform>(item.entity);
            auto& renderer = m_Registry->get<Renderer>(item.entity);

            if (renderer.m_Texture == nullptr)
            {
                continue;
            }
            sf::Sprite sprite(*(renderer.m_Texture));

            sprite.setColor(renderer.m_Color);
            sprite.setOrigin(transform.m_Origin);
            sprite.setPosition(transform.m_Position);
            sprite.setScale(transform.m_Scale);
            sprite.setRotation(transform.m_Rotation);
            
            // Animation (optionnel)
            if (m_Registry->try_get<Animated>(item.entity))
            {
                auto& anim = m_Registry->get<Animated>(item.entity);
                ApplyAnimation(sprite, anim, renderer.m_Texture);
            }

            m_Window->draw(sprite);
        }
    }

    void RenderSystem::ApplyAnimation(sf::Sprite& sprite, Animated& anim, sf::Texture* texture)
    {
        if (!texture || anim.m_FramesX * anim.m_FramesY <= 1)
            return;

        // Avance du temps
        anim.m_CurrentTime += Time::GetDeltaTime().asSeconds();
        if (anim.m_CurrentTime >= anim.m_Duration)
            anim.m_CurrentTime -= anim.m_Duration;

        // Durée d'une frame
        const int totalFrames = anim.m_FramesX * anim.m_FramesY;
        float frameDuration = anim.m_Duration / totalFrames;

        // Index de frame
        int frameIndex = static_cast<int>(anim.m_CurrentTime / frameDuration);

        // Calcul X/Y
        int frameX = frameIndex % anim.m_FramesX;
        int frameY = frameIndex / anim.m_FramesX;

        // Taille d'une frame
        int frameWidth = texture->getSize().x / anim.m_FramesX;
        int frameHeight = texture->getSize().y / anim.m_FramesY;

        sprite.setTextureRect(sf::IntRect(
            { frameX * frameWidth, frameY * frameHeight},
            { frameWidth, frameHeight }
        ));
    }

}