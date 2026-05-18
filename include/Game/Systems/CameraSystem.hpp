#pragma once

#include <Game/Systems.hpp>

namespace Game
{
	class CameraSystem : public SystemBase
	{
	public:

		const float CamMoveSpeed = 500.f;
		const float CamZoomSpeed = 0.1f;
		const float CamZoomMin = 0.5f;
		const float CamZoomMax = 3.f;

		CameraSystem(pallas::World* world, pallas::Logger& logger);

		~CameraSystem() { };

		virtual void OnCreate() override;

		virtual pallas::UpdatePhase Phase() const {return pallas::UpdatePhase::LateUpdate; };

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose) override;

		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose) override;

		virtual void OnEnabled(bool enabled) override;

		virtual void Update() override;

	private:
		pallas::InputManager &m_inputManager = pallas::InputManager::Instance();;
		pallas::InputBindingState* m_moveUpBinding = nullptr;
		pallas::InputBindingState* m_moveDownBinding = nullptr;
		pallas::InputBindingState* m_moveLeftBinding = nullptr;
		pallas::InputBindingState* m_moveRightBinding = nullptr;
		pallas::InputBindingState* m_zoomBinding = nullptr;
	};
}