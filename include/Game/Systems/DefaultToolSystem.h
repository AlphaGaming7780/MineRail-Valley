#pragma once

#include <SFML/Graphics.hpp>

#include <Game/Systems/SystemBase.hpp>
#include <Game/Events/InputManager.hpp>
#include <Game/GameObjects/Tracks/TrackObject.h>
#include <Game/GameObjects/Trains/TrainObject.h>

namespace Game
{
	class DefaultToolSystem : public SystemBase
	{
	public:

		static const sf::Color kHoverColor;
		//static const sf::Color kSelectColor;

		DefaultToolSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}
		
		void OnCreate() override;
		void OnUpdate() override;
		void OnDestroy() override;

		void OnEnabled(bool enabled) override;

		virtual void OnGameLoadingStart(GameMode mode, Purpose purpose, MapData* mapData) override;
		virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose, MapData* mapData) override;

	private:
		InputManager& m_InputManager = InputManager::Instance();
		InputBindingState* m_LeftToggleBinding = nullptr;
		InputBindingState* m_RightToggleBinding = nullptr;

		TrackObject* m_SelectedTrack = nullptr;
		TrainObject* m_SelectedTrain = nullptr;

		std::vector<TrackObjectBase*> m_SelectedTrackConnections;

		void Update();
		void LeftToggleActions();
		void RightToggleActions();
		void Cleanup();
		void ToggleTrack(int direction);
		void ToggleTrain();
	};

	inline constexpr sf::Color DefaultToolSystem::kHoverColor(128, 255, 255, 255); //(128, 183, 235, 255);
	//inline constexpr sf::Color DefaultToolSystem::kSelectColor(128, 255, 255, 255);

}