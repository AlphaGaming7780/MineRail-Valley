#include <Game/Systems/TrainSystem.h>
#include <Game/AssetDatabase/StationDatabase.h>

namespace Game
{
	void TrainSystem::OnCreate()
	{
		m_StationsData.emplace("Stations/black_station.json", StationDatabase::Instance().Load("Stations/black_station.json"));
		m_StationsData.emplace("Stations/blue_station.json", StationDatabase::Instance().Load("Stations/black_station.json"));
		m_StationsData.emplace("Stations/purple_station.json", StationDatabase::Instance().Load("Stations/purple_station.json"));
		m_StationsData.emplace("Stations/red_station.json", StationDatabase::Instance().Load("Stations/red_station.json"));
		m_StationsData.emplace("Stations/yellow_station.json", StationDatabase::Instance().Load("Stations/yellow_station.json"));



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
		
	}



}