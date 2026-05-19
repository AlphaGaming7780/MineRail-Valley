#pragma once

#include <Game/Events/InputManager.hpp>
#include <Game/Systems.hpp>
#include <Game/GameStates.h>

namespace Game
{
	class CameraSystem : public SystemBase
	{
	public:

		const float CamMoveSpeed = 500.f;
		const float CamZoomSpeed = 0.1f;
		const float CamZoomMin = 0.5f;
		const float CamZoomMax = 3.f;

		CameraSystem(World* world, pallas::Logger& logger);

		~CameraSystem() { };

		virtual void OnCreate() override;

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose) override;

		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose) override;

		virtual void OnEnabled(bool enabled) override;

		virtual void Update() override;

	private:
		InputManager &m_inputManager = InputManager::Instance();
		InputBindingState* m_moveUpBinding = nullptr;
		InputBindingState* m_moveDownBinding = nullptr;
		InputBindingState* m_moveLeftBinding = nullptr;
		InputBindingState* m_moveRightBinding = nullptr;
		InputBindingState* m_zoomBinding = nullptr;
	};
}