#pragma once

#include <Game/Systems/SystemBase.hpp>
#include <Game/Systems/DefaultToolSystem.h>
#include <Game/Events/InputManager.hpp>
#include <Game/GameObjects/Tiles/TileObject.h>
#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/World.h>

namespace Game
{
	class TrackToolSystem : public SystemBase
	{
	public:

		static const sf::Color kBuildColor;

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

		TileObject* m_StartTileObject = nullptr;
		TileObject* m_EndTileObject = nullptr;

		std::vector<TileObject*> m_TilePath;
		std::vector<TrackObject*> m_TrackPath;

		void Update();
		void Apply();
		void Cancel();
		void Cleanup();

		void SetState(State state);

	};

	inline constexpr sf::Color TrackToolSystem::kBuildColor(128, 255, 255, 255);
}