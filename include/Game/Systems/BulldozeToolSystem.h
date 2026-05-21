#pragma once

#include <Game/Systems/SystemBase.hpp>

namespace Game
{
	class BulldozeToolSystem : public SystemBase
	{
	public:

		static const sf::Color kBulldozeColorHover;
		static const sf::Color kBulldozeColor;

		BulldozeToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		enum State
		{
			Default,
			Destroying
		};

		void OnCreate() override;
		void OnUpdate() override;
		void OnDestroy() override;

		void OnEnabled(bool enabled) override;

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose, MapData* mapData) override;
		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose, MapData* mapData) override;

	private:
		InputManager& m_InputManager = InputManager::Instance();
		InputBindingState* m_ApplyBinding = nullptr;
		InputBindingState* m_CancelBinding = nullptr;

		State m_State = State::Default;

		DefaultToolSystem* m_DefaultToolSystem = nullptr;

		TrackObject* m_StartTrackObject = nullptr;
		TrackObject* m_EndTrackObject = nullptr;

		std::vector<TrackObject*> m_TrackPath;

		void Update();
		void Apply();
		void Cancel();
		void Cleanup();

		void SetState(State state);
	};

	inline constexpr sf::Color BulldozeToolSystem::kBulldozeColorHover(255, 165, 128, 255);
	inline constexpr sf::Color BulldozeToolSystem::kBulldozeColor(255, 128, 128, 255);
}