#include <Game/Systems/CameraSystem.hpp>
#include <magic_enum/magic_enum.hpp>

#include <Game/InputsActionSets/CameraInputSet.hpp>

namespace Game
{
	CameraSystem::CameraSystem(pallas::World* world, pallas::Logger& logger) : SystemBase(world, logger)
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

	void CameraSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		m_Logger.InfoO("CameraSystem received OnGameLoadingStart, GameMode: ", magic_enum::enum_name(mode), " | Purpose: ", magic_enum::enum_name(purpose));

		SetEnable(false);
	}

	void CameraSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
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

	void CameraSystem::Update()
	{
		entt::entity e = GetOrCreateSingletonEntity<pallas::CameraComponent>();
		pallas::CameraComponent& cam = m_Registry->get<pallas::CameraComponent>(e);

		const float dt = pallas::Time::GetDeltaTime().asSeconds();

		if (m_zoomBinding->wheelDelta != 0.f)
		{
			cam.zoom -= m_zoomBinding->wheelDelta * 0.1f;
			cam.zoom = std::clamp(cam.zoom, CamZoomMin, CamZoomMax);
		}

		sf::Vector2f dir{ 0.f, 0.f };

		if (m_moveUpBinding->pressed)		dir.y -= 1.f;
		if (m_moveDownBinding->pressed)		dir.y += 1.f;
		if (m_moveLeftBinding->pressed)     dir.x -= 1.f;
		if (m_moveRightBinding->pressed)    dir.x += 1.f;

		if(dir == sf::Vector2f{ 0.f, 0.f }) return;

		cam.position += dir.normalized() * (CamMoveSpeed * dt * cam.zoom);
	}
}