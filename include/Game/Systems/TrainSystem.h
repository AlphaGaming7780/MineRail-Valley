#pragma once

#include <Game/Systems/SystemBase.hpp>
#include <Game/World.h>
#include <Game/AssetDatabase/AssetType/StationData.h>
//#include <Game/AssetDatabase/AssetType/TrainData.h>
#include <Game/GameObjects/Tracks/StationObject.h>
#include <Game/GameObjects/Trains/TrainObject.h>
#include <Game/GameObjects/Tiles/TileObject.h>

namespace Game
{
	class TrainSystem : public SystemBase
	{
	public:
		TrainSystem(World* world, pallas::Logger& logger)
			: SystemBase(world, logger)
		{
		}

		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate() override;

		void OnGameLoadingStart(GameMode mode, Purpose purpose) override;
		void OnGameLoadingComplete(GameMode mode, Purpose purpose) override;

	private:

		bool m_InWave = 0;
		float m_StartWaveTimer = 0;

		TrackData* m_TrackData = nullptr;
		std::unordered_map<std::string, StationData*> m_StationsData;
		//std::unordered_map<std::string, TrainData*> m_StationsData;

		std::vector<StationObject*> m_Stations;
		std::vector<TrainObject*> m_Trains;
		std::vector<StationData*> m_StationSpawnPool;

		StationObject* SpawnStation(StationData* data, TileObject* tile);
		TrainObject* SpawnTrain(StationObject* src, StationObject* dst);
		//void DestroyTrain(TrainObject* train);

		void SpawnInitialStations();
		void SpawnWaveTrains();
		bool AllTrainsArrived() const;
		//void ClearWaveTrains();

		TileObject* GetStationSpawnTile() const;
		StationObject* SpawnStationFromPool();
		TrackObject* SpawnTrackOnTile(TileObject* tile);

	};
}