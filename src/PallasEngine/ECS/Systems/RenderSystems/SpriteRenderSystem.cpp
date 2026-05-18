#include <PallasEngine/ECS/Systems/RenderSystems/SpriteRenderSystem.hpp>
#include <PallasEngine/ECS/Components/Transform.hpp>
#include <PallasEngine/ECS/Components/Renderer.hpp>
#include <PallasEngine/ECS/Components/Animated.hpp>
#include <PallasEngine/Utils/Time.hpp>

namespace pallas
{
	void SpriteRenderSystem::Update()
    {
        m_Commands.clear();

        entt::entity cam = GetOrCreateSingletonEntity<CameraComponent>();
        CameraComponent camComp = m_Registry->get<CameraComponent>(cam);

        auto view = m_Registry->view<Transform, Renderer>();

        for (auto entity : view)
        {
            auto& tr = view.get<Transform>(entity);
            auto& rd = view.get<Renderer>(entity);

            sf::Rect<float> objBounds = TransformUtils::GetGlobalBounds(tr);

            const sf::Rect<float>& camBounds = camComp.globalBounds;

            if (!objBounds.findIntersection(camBounds).has_value())
                continue;

            if (!rd.visible || rd.m_Texture == nullptr)
                continue;

            RenderCommand cmd;
            cmd.type = RenderCommand::Type::Sprite;
            cmd.texture = rd.m_Texture;
            cmd.color = rd.m_Color;
            cmd.position = tr.m_Position;
            cmd.origin = tr.m_Origin;
            cmd.size = tr.m_Size;
            cmd.scale = tr.m_Scale;
            cmd.rotation = tr.m_Rotation;
            cmd.zIndex = rd.m_zIndex;

            // Animation ?
            if (auto anim = m_Registry->try_get<Animated>(entity))
            {
                auto frame = AnimatedTextureUtils::ComputeFrame(
                    *anim,
                    rd.m_Texture,
                    Time::GetDeltaTime().asSeconds()
                );

                if (frame.valid)
                {
                    cmd.animated = true;
                    cmd.frameX = frame.frameX;
                    cmd.frameY = frame.frameY;
                    cmd.frameWidth = frame.frameWidth;
                    cmd.frameHeight = frame.frameHeight;
                }
            }

            m_Commands.push_back(cmd);
        }

        // Pour teste le culling plus tard.
        //m_Logger.InfoO("Size of m_Commands: ", m_Commands.size());

    }
}