#pragma once

#include <Game/Systems/SystemBase.hpp>
#include <Game/Systems/DefaultToolSystem.h>
#include <Game/Events/InputManager.hpp>
#include <Game/World.h>

namespace Game
{
	class TrackToolSystem : public SystemBase
	{
	public:

		TrackToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		enum State
		{
			Default,
			Creating
		};

		void OnCreate() override;
		void Update() override;
		void OnDestroy() override;

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose) override;
		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose) override;

	private:
		InputManager& m_InputManager = InputManager::Instance();
		InputBindingState* m_ApplyBinding = nullptr;
		InputBindingState* m_CancleBinding = nullptr;

		State m_State = State::Default;

		DefaultToolSystem* m_DefaultToolSystem = nullptr;

	};
}