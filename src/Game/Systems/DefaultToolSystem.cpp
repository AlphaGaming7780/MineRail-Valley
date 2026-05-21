#pragma once

#include<Game/Systems.hpp>
#include<Game/InputsActionSets/TrackToolInputSet.h>
#include <Game/Utils/RaycastUtils.h>

namespace Game
{
	void DefaultToolSystem::OnCreate()
	{
		SystemBase::OnCreate();

		m_InputManager.Register<TrackInputs>();
		m_LeftToggleBinding = &m_InputManager.GetBinding(TrackToolAction::Apply);
		m_RightToggleBinding = &m_InputManager.GetBinding(TrackToolAction::Cancel);
		SetEnable(true);
	}
	void DefaultToolSystem::OnUpdate()
	{
		if (UIManager::Instance().IsMouseOverUI())
		{
			Cleanup();
			return;
		}

		if (m_LeftToggleBinding->justPressed)
		{
			LeftToggleActions();
			return;
		}
		else if (m_RightToggleBinding->justPressed)
		{
			RightToggleActions();
			return;
		}

		Update();
	}
	void DefaultToolSystem::OnDestroy()
	{
		Cleanup();
	}
	void DefaultToolSystem::OnEnabled(bool enabled)
	{
		m_InputManager.EnableBlock<TrackToolAction>(enabled);
		Cleanup();
	}
	void DefaultToolSystem::OnGameLoadingStart(GameMode mode, Purpose purpose, MapData* mapData)
	{
		SetEnable(false);
	}
	void DefaultToolSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose, MapData* mapData)
	{
		SetEnable(false);
	}
	void DefaultToolSystem::Update()
	{
		GameObject* result = RaycastUtils::PerformRaycast(*m_World, GameWindow::Instance().Get());

		auto train = dynamic_cast<TrainObject*>(result);
		auto track = dynamic_cast<TrackObject*>(result);

		if (train)
		{
			if (train != m_SelectedTrain)
			{
				if (m_SelectedTrain) m_SelectedTrain->ResetColor();
				if (m_SelectedTrack) m_SelectedTrack->ResetColor();
				m_SelectedTrack = nullptr;
				m_SelectedTrain = train;
				m_SelectedTrain->SetColor(kHoverColor);
			}
			return;
		}
		else if (track && track->m_CanBeDeleted)
		{
			
			m_SelectedTrackConnections = track->GetAdjacentTracks();
			if (m_SelectedTrackConnections.size() > 2) 
			{
				if (m_SelectedTrain) m_SelectedTrain->ResetColor();
				if (m_SelectedTrack) m_SelectedTrack->ResetColor();
				m_SelectedTrain = nullptr;
				m_SelectedTrack = track;
				m_SelectedTrack->SetColor(kHoverColor);
				return;
			}
		}

		Cleanup();
		return;

	}
	void DefaultToolSystem::LeftToggleActions()
	{
		if (m_SelectedTrain)
		{
			ToggleTrain();
		}
		else if (m_SelectedTrack)
		{
			ToggleTrack(1);
		}
	}
	void DefaultToolSystem::RightToggleActions()
	{
		if (m_SelectedTrain)
		{
			m_SelectedTrain->m_Enabled = !m_SelectedTrain->m_Enabled;
		}
		else if (m_SelectedTrack)
		{
			ToggleTrack(-1);
		}
	}
	void DefaultToolSystem::Cleanup()
	{
		if (m_SelectedTrack) m_SelectedTrack->ResetColor();
		if (m_SelectedTrain) m_SelectedTrain->ResetColor();
		m_SelectedTrack = nullptr;
		m_SelectedTrain = nullptr;

		m_SelectedTrackConnections.clear();
	}

	void DefaultToolSystem::ToggleTrack(int direction)
	{
		if (!m_SelectedTrack) return;

		auto& connections = m_SelectedTrackConnections;
		int size = (int)connections.size();

		if (size <= 2) return;

		using Pair = std::pair<TrackObjectBase*, TrackObjectBase*>;

		Pair actualPair(m_SelectedTrack->m_First, m_SelectedTrack->m_Second);

		// construire les paires
		std::vector<Pair> pairs;

		for (int i = 0; i < size; i++)
		{
			for (int j = i + 1; j < size; j++)
			{
				pairs.emplace_back(connections[i], connections[j]);
			}
		}

		if (pairs.empty()) return;

		// trouver l'état actuel (ordre non important)
		int currentIndex = -1;

		for (int i = 0; i < (int)pairs.size(); i++)
		{
			auto& p = pairs[i];

			if ((p.first == actualPair.first && p.second == actualPair.second) ||
				(p.first == actualPair.second && p.second == actualPair.first))
			{
				currentIndex = i;
				break;
			}
		}

		if (currentIndex == -1) return;

		int count = (int)pairs.size();

		// appliquer direction (+1 ou -1)
		int newIndex = (currentIndex + direction + count) % count;

		auto& newPair = pairs[newIndex];
		m_SelectedTrack->ConnectTrack(newPair.first, newPair.second);
	}

	void DefaultToolSystem::ToggleTrain()
	{
		m_Logger.Info("HelloToggle");
		TrackObjectBase* tmp = m_SelectedTrain->m_Current;
		m_SelectedTrain->m_Current = m_SelectedTrain->m_Next;
		m_SelectedTrain->m_Next = tmp;
	}

}