#pragma once

#include <Game/Systems/SystemBase.hpp>
#include <Game/AssetDatabase/AssetType/StationData.h>
//#include <Game/AssetDatabase/AssetType/TrainData.h>

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

	private:
		std::unordered_map<std::string, StationData*> m_StationsData;
		//std::unordered_map<std::string, TrainData*> m_StationsData;

	};
}