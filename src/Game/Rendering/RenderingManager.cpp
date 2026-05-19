#include <PallasEngine/Rendering/RenderingManager.hpp>

namespace pallas
{
    RenderingManager& RenderingManager::Instance()
    {
        static RenderingManager instance;
        return instance;
    }

    void RenderingManager::RegisterQueue(RenderSystemBase* queue)
    {
        m_Queues.push_back(queue);
    }

    void RenderingManager::Render(sf::RenderWindow& window)
    {
        m_Camera.SetCenter(m_CameraState.position);
        m_Camera.SetRotation(m_CameraState.rotation);
        m_Camera.SetZoom(m_CameraState.zoom);
        //m_Camera.SetSize(m_CameraState.size);

        window.setView(m_Camera.GetView());


        m_Merged.clear();

        // Fusionner toutes les queues dans un seul buffer
        for (auto* q : m_Queues)
        {
            auto cmds = q->GetCommands();
            m_Merged.insert(m_Merged.end(), cmds.begin(), cmds.end());
        }

        // Tri global par zIndex
        std::sort(m_Merged.begin(), m_Merged.end(),
            [](const RenderCommand& a, const RenderCommand& b)
            {
                return a.zIndex < b.zIndex;
            });

        // Draw
        for (auto& cmd : m_Merged)
        {
            if (cmd.type == RenderCommand::Type::Sprite)
            {
                if (!cmd.texture) continue;

                sf::Sprite sprite(*cmd.texture);
                sprite.setColor(cmd.color);
                sprite.setPosition(cmd.position);
                sprite.setOrigin(cmd.origin);
                sprite.setScale(GetActualScale(cmd));
                sprite.setRotation(cmd.rotation);

                if (cmd.animated)
                {
                    sprite.setTextureRect(sf::IntRect({
                        {
                            cmd.frameX * cmd.frameWidth,
                            cmd.frameY * cmd.frameHeight,
                        },{
                            cmd.frameWidth,
                            cmd.frameHeight
                        }
                    }));
                }

                window.draw(sprite);
            }
        }

        // Clear des queues (optionnel, sinon les systèmes le font eux-mêmes)
        for (auto* q : m_Queues)
            q->ClearCommands();
    }

    const CameraComponent RenderingManager::GetCameraState() const
    {
        CameraComponent s;

        const auto& view = m_Camera.GetView();

        s.position = view.getCenter();
        s.size = view.getSize();
        s.baseSize = m_Camera.GetBaseSize();
        s.rotation = view.getRotation();
        s.zoom = m_Camera.GetZoom();
        s.globalBounds = m_Camera.GetGlobalBounds();
        s.localBounds = m_Camera.GetLocalBounds();
        return s;
    }

    void RenderingManager::SetCameraState(const CameraComponent& state)
    {
        m_CameraState = state;
    }

    sf::Vector2f RenderingManager::GetActualScale(const RenderCommand& cmd)
    {
        if (!cmd.texture)
            return { 0.f, 0.f };

        // Taille brute de la texture ou du frame animé
        sf::Vector2f baseSize;

        if (cmd.animated)
        {
            baseSize = sf::Vector2f(
                static_cast<float>(cmd.frameWidth),
                static_cast<float>(cmd.frameHeight)
            );
        }
        else
        {
            auto texSize = cmd.texture->getSize();
            baseSize = sf::Vector2f(
                static_cast<float>(texSize.x),
                static_cast<float>(texSize.y)
            );
        }

        // Éviter division par zéro
        if (baseSize.x == 0.f || baseSize.y == 0.f)
            return { 0.f, 0.f };

        // Scale absolu (size / textureSize)
        sf::Vector2f absoluteScale = {
            cmd.size.x / baseSize.x,
            cmd.size.y / baseSize.y
        };

        // Scale final = scale absolu * scale SFML
        return {
            absoluteScale.x * cmd.scale.x,
            absoluteScale.y * cmd.scale.y
        };
    }

}