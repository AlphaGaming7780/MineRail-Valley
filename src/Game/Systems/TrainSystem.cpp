#include <Game/Systems/TrainSystem.h>
#include <Game/AssetDatabase/StationDatabase.h>
#include <Game/AssetDatabase/TrainDatabase.h>
#include <Game/AssetDatabase/TrackDatabase.h>
#include <Game/Utils/Time.hpp>
#include <random>

namespace Game
{
	void TrainSystem::OnCreate()
	{
		m_TrackData = TrackDatabase::Instance().Load("Tracks\\BaseTrack.json");
		m_StationsData.emplace("Stations/black_station.json", StationDatabase::Instance().Load("Stations/black_station.json"));
		m_StationsData.emplace("Stations/blue_station.json", StationDatabase::Instance().Load("Stations/blue_station.json"));
		m_StationsData.emplace("Stations/purple_station.json", StationDatabase::Instance().Load("Stations/purple_station.json"));
		m_StationsData.emplace("Stations/red_station.json", StationDatabase::Instance().Load("Stations/red_station.json"));
		m_StationsData.emplace("Stations/yellow_station.json", StationDatabase::Instance().Load("Stations/yellow_station.json"));

		m_TrainsData.emplace("Trains/Minecart.json", TrainDatabase::Instance().Load("Trains/Minecart.json"));

	}

	void TrainSystem::OnDestroy()
	{
		for (auto& [p, ob] : m_StationsData)
		{
			StationDatabase::Instance().Unload(p);
		}
		m_StationsData.clear();
	}

	void TrainSystem::OnUpdate()
	{
		if (m_InWave)
		{
			if (AllTrainsArrived())
			{
				m_InWave = false;
				if (m_StationSpawnPool.size() <= 0)
				{
					// Remplacer par la victoire
					SetEnable(false);
				}
				SpawnStationFromPool();
			}
			return;
		}
		else
		{
			if (m_StartWaveTimer < 10)
			{
				m_StartWaveTimer += Time::GetDeltaTime().asSeconds();
				return;
			}
			m_InWave = true;
			m_StartWaveTimer = 0;

			SpawnWaveTrains();

		}
	}

	void TrainSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
		m_StationSpawnPool.clear();
		if (mode == GameMode::InGame)
		{
			m_StationSpawnPool.clear();
			m_StationSpawnPool.reserve(m_StationsData.size());

			for (auto& [pos, data] : m_StationsData)
				m_StationSpawnPool.push_back(data);

			static thread_local std::mt19937 rng(std::random_device{}());

			std::shuffle(
				m_StationSpawnPool.begin(),
				m_StationSpawnPool.end(),
				rng
			);
		}
	}

	void TrainSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
	{
		if (mode == GameMode::InGame)
		{
			SetEnable(true);
			SpawnInitialStations();
			m_InWave = false;
		}
	}

	void TrainSystem::OnEvent(const TrainStopped& event)
	{
		TrainObject* train = event.m_Train;

		if (!train->m_Current)
		{
			DestroyTrain(train);
			return;
		}

		if (train->m_Current == train->m_StationDest)
		{
			DestroyTrain(train);
			return;
		}

		// Perdu la game

		DestroyTrain(train);
	}

	TileObject* TrainSystem::GetStationSpawnTile() const
	{
		const std::vector<TileObject*> tiles = m_World->GetAllGameObject<TileObject>();
		std::vector<TileObject*> candidates;
		candidates.reserve(tiles.size());

		for (TileObject* tile : tiles)
		{
			if (!tile->m_CanBuild || tile->m_PlacedTrack != nullptr)
				continue;

			bool valid = true;

			for (TileObject* adj : tile->GetAdjacentTiles())
			{
				if (!adj->m_CanBuild || adj->m_PlacedTrack != nullptr)
				{
					valid = false;
					break;
				}
			}

			if (valid)
				candidates.push_back(tile);
		}

		if (candidates.empty())
			return nullptr;

		static thread_local std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);

		return candidates[dist(rng)];
	}

	StationObject* TrainSystem::SpawnStationFromPool()
	{
		if (m_StationSpawnPool.empty())
			return nullptr;

		StationData* data = m_StationSpawnPool.back();

		TileObject* tile = GetStationSpawnTile();
		if (!tile)
			return nullptr;

		StationObject* station = SpawnStation(data, tile);
		if (!station)
			return nullptr;

		m_StationSpawnPool.pop_back();

		return station;
	}

	TrackObject* TrainSystem::SpawnTrackOnTile(TileObject* tile)
	{
		if (!tile) return nullptr;
		if (!tile->m_CanBuild) return nullptr;
		if (tile->m_PlacedTrack) return nullptr;

		TrackObject* track = m_World->CreateGameObject<TrackObject>(*m_TrackData);

		track->SetTile(tile);
		track->m_Enabled = true;
		track->m_CanBeDeleted = false;

		return track;
	}

	StationObject* TrainSystem::SpawnStation(StationData* data, TileObject* tile)
	{
		if (!tile || !data) return nullptr;

		StationObject* station = m_World->CreateGameObject<StationObject>(*data);
		station->SetTile(tile);
		station->SetPosition(tile->GetPosition());
		station->m_Enabled = true;

		std::vector<TileObject*> tiles = tile->GetAdjacentTiles();
		static thread_local std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<size_t> dist(0, tiles.size() - 1);

		tile = tiles[dist(rng)];
		station->m_AllowedTileConnection = tile->m_Index;

		TrackObject* track = SpawnTrackOnTile(tile);
		if (!track)
		{
			m_World->DestroyObject(station);
			return nullptr;
		}

		station->m_First = track;

		m_Stations.push_back(station);
		return station;
	}

	TrainObject* TrainSystem::SpawnTrain(StationObject* src, StationObject* dst)
	{
		if (!src || !dst || m_TrainsData.empty()) return nullptr;

		constexpr float kDefaultTrainSpeed = 200.f; // px/s

		std::vector<TrainData*> list;
		list.reserve(m_TrainsData.size());
		for (auto& [name, data] : m_TrainsData)
			list.push_back(data);

		static thread_local std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<size_t> dist(0, list.size() - 1);

		TrainObject* train = m_World->CreateGameObject<TrainObject>(*list[dist(rng)]);
		train->m_StationSrc = src;
		train->m_StationDest = dst;
		train->m_Current = src;
		train->m_Next = src->m_First;
		train->m_Speed = kDefaultTrainSpeed;
		train->m_Color = dst->m_StationColor;
		train->ResetColor();

		if (src->m_Tile)
			train->SetPosition(src->m_Tile->GetPosition());

		train->m_RenderLayer = RenderLayer::Trains;
		train->m_Enabled = true;

		m_Trains.push_back(train);
		return train;
	}

	void TrainSystem::DestroyTrain(TrainObject* train)
	{ 
		if (!train)
			return;

		for (WagonObject* wagon : train->m_Wagons)
		{
			if (wagon)
				m_World->DestroyObject(wagon);
		}

		train->m_Wagons.clear();
		auto it = std::find(m_Trains.begin(), m_Trains.end(), train);
		if (it != m_Trains.end())
			m_Trains.erase(it);

		m_World->DestroyObject(train);
	}

	void TrainSystem::SpawnInitialStations()
	{
		SpawnStationFromPool();
		SpawnStationFromPool();
	}

	void TrainSystem::SpawnWaveTrains()
	{
		const int stationCount = static_cast<int>(m_Stations.size());
		if (stationCount < 2)
			return;

		// Préparer un RNG propre
		static thread_local std::mt19937 rng(std::random_device{}());

		// Pour chaque station → spawn un train
		for (int i = 0; i < stationCount; ++i)
		{
			StationObject* src = m_Stations[i];
			StationObject* dst = nullptr;

			// Construire une liste des stations compatibles (couleur différente)
			std::vector<StationObject*> candidates;
			candidates.reserve(stationCount);

			for (int j = 0; j < stationCount; ++j)
			{
				if (i == j) continue;
				if (m_Stations[j]->m_StationColor != src->m_StationColor)
					candidates.push_back(m_Stations[j]);
			}

			// Si aucune station compatible → skip
			if (candidates.empty())
				continue;

			// Tirage aléatoire d’une destination
			std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
			dst = candidates[dist(rng)];

			// Spawn du train
			SpawnTrain(src, dst);
		}
	}


	bool TrainSystem::AllTrainsArrived() const
	{
		if (m_Trains.empty()) return true;
		return false;
	}
}