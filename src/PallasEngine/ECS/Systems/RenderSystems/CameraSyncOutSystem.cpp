#include <PallasEngine/ECS/Systems/RenderSystems/CameraSyncOutSystem.hpp>
#include <PallasEngine/ECS/Components/CameraComponent.hpp>
#include <PallasEngine/Rendering/RenderingManager.hpp>

namespace pallas
{
    void CameraSyncOutSystem::OnCreate()
    {
        m_CemeraEntity = GetOrCreateSingletonEntity<CameraComponent>();
    }

    void CameraSyncOutSystem::Update()
    {
        auto& rm = RenderingManager::Instance();
        //RenderingManager::CameraState camState;

        auto& cam = m_Registry->get<CameraComponent>(m_CemeraEntity);
        rm.SetCameraState(cam);

        //camState.position = cam.position;
        //camState.zoom = cam.zoom;
        //camState.rotation = cam.rotation;
        //camState.baseSize = cam.baseSize;
        //camState.size = cam.size;

        //rm.SetCameraState(camState);
    }
}
