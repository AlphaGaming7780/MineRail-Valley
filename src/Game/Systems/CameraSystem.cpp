#include <Game/Systems/CameraSystem.hpp>
#include <magic_enum/magic_enum.hpp>

#include <Game/InputsActionSets/CameraInputSet.hpp>
#include <Game/Utils/Time.hpp>
#include <Game/Rendering/Camera.hpp>

namespace Game
{
	CameraSystem::CameraSystem(World* world, pallas::Logger& logger) : SystemBase(world, logger)
	{

	}

	void CameraSystem::OnCreate()
	{
		SystemBase::OnCreate();
		m_inputManager.Register<CameraInputs>();
		m_moveUpBinding =		&m_inputManager.GetBinding(CameraAction::MoveUp);
		m_moveDownBinding =		&m_inputManager.GetBinding(CameraAction::MoveDown);
		m_moveLeftBinding =		&m_inputManager.GetBinding(CameraAction::MoveLeft);
		m_moveRightBinding =	&m_inputManager.GetBinding(CameraAction::MoveRight);
		m_zoomBinding =			&m_inputManager.GetBinding(CameraAction::Zoom);
	}

	void CameraSystem::OnGameLoadingStart(GameMode mode, Purpose purpose, MapData* mapData)
	{
		m_Logger.InfoO("CameraSystem received OnGameLoadingStart, GameMode: ", magic_enum::enum_name(mode), " | Purpose: ", magic_enum::enum_name(purpose));

        if(mapData)
            m_MapSize = {
                mapData->MapSize* 100.f,
                mapData->MapHeight * 100.f
            };

		SetEnable(false);
	}

	void CameraSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose, MapData* mapData)
	{
		m_Logger.InfoO("CameraSystem received OnGameLoadingComplete, GameMode: ", magic_enum::enum_name(mode), " | Purpose: ", magic_enum::enum_name(purpose));

		if (mode == GameMode::InGame)	
			SetEnable(true);
	}

	void CameraSystem::OnEnabled(bool enabled)
	{
		m_Logger.InfoO("CameraSystem ", (enabled ? "enabled" : "disabled"));
		m_inputManager.EnableBlock<CameraAction>(enabled);
	}

    void CameraSystem::OnUpdate()
    {
        Camera& cam = Camera::Instance();

        float zoom = cam.GetZoom();
        const float dt = Time::GetDeltaTime().asSeconds();

        // Zoom
        if (m_zoomBinding->wheelDelta != 0.f)
        {
            zoom -= m_zoomBinding->wheelDelta * 0.1f;
            zoom = std::clamp(zoom, CamZoomMin, CamZoomMax);
            cam.SetZoom(zoom);
        }

        // Direction
        sf::Vector2f dir{ 0.f, 0.f };
        if (m_moveUpBinding->pressed)    dir.y -= 1.f;
        if (m_moveDownBinding->pressed)  dir.y += 1.f;
        if (m_moveLeftBinding->pressed)  dir.x -= 1.f;
        if (m_moveRightBinding->pressed) dir.x += 1.f;

        if (dir == sf::Vector2f{ 0.f, 0.f })
            return;

        // Nouveau centre
        sf::Vector2f newCenter =
            cam.GetCenter() + dir.normalized() * (CamMoveSpeed * dt * zoom);

        // -----------------------------
        // CLAMP DU CENTRE DE LA CAMÉRA
        // -----------------------------
        newCenter.x = std::clamp(newCenter.x, 0.f, m_MapSize.x);
        newCenter.y = std::clamp(newCenter.y, 0.f, m_MapSize.y);

        cam.SetCenter(newCenter);
    }
}