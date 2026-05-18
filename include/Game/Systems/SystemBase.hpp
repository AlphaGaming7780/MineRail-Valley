#pragma once

#include <PallasEngine/PallasEngine.hpp>
#include <Game/GameManager.hpp>

namespace Game
{
	class SystemBase : public pallas::SystemBase
	{
	public:
		SystemBase(pallas::World* world, entt::registry* registry, pallas::Logger& logger) : pallas::SystemBase(world, registry, logger) {}
		~SystemBase() {};

		void OnCreate() override 
		{
			GameManager& gm = GameManager::Instance();
			gm.OnGameLoadingComplete += pallas::Delegate<GameMode, Purpose>(this, &Game::SystemBase::OnGameLoadingStart);
			gm.OnGameLoadingComplete += pallas::Delegate<GameMode, Purpose>(this, &Game::SystemBase::OnGameLoadingComplete);
			m_Logger.InfoO("SystemBase subscribed to GameManager events.");
		};

		virtual void OnDestroy() override 
		{
			GameManager& gm = GameManager::Instance();
			gm.OnGameLoadingComplete -= pallas::Delegate<GameMode, Purpose>(this, &Game::SystemBase::OnGameLoadingStart);
			gm.OnGameLoadingComplete -= pallas::Delegate<GameMode, Purpose>(this, &Game::SystemBase::OnGameLoadingComplete);
			m_Logger.InfoO("SystemBase unsubscribed from GameManager events.");
		};

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose) {};
		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose) {};

	private:

	};
}