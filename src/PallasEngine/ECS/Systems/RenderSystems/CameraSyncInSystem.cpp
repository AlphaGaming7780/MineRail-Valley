#include <PallasEngine/ECS/Systems/RenderSystems/CameraSyncInSystem.hpp>
#include <PallasEngine/ECS/Components/CameraComponent.hpp>
#include <PallasEngine/Rendering/RenderingManager.hpp>

namespace pallas
{
    void CameraSyncInSystem::OnCreate()
    {
        m_CemeraEntity = GetOrCreateSingletonEntity<CameraComponent>();
    }
    void CameraSyncInSystem::Update()
    {
        auto& rm = RenderingManager::Instance();
        const auto& camState = rm.GetCameraState();
        m_Registry->replace<CameraComponent>(m_CemeraEntity, camState);

        //auto& cam = m_Registry->get<CameraComponent>(m_CemeraEntity);
        //cam.position = camState.position;
        //cam.zoom	 = camState.zoom;
        //cam.rotation = camState.rotation;
        //cam.baseSize = camState.baseSize;
        //cam.size	 = camState.size;
    }
}
