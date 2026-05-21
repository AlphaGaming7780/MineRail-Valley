#pragma once
#include<Game/Systems.hpp>
#include<Game/InputsActionSets/TrackToolInputSet.h>
#include <Game/Utils/RaycastUtils.h>
#include <Game/Utils/TrackUtils.h>

namespace Game
{
	void BulldozeToolSystem::OnCreate()
	{
		SystemBase::OnCreate();

		m_DefaultToolSystem = m_World->GetOrCreateSystem<DefaultToolSystem>();

		m_InputManager.Register<TrackInputs>();
		m_ApplyBinding = &m_InputManager.GetBinding(TrackToolAction::Apply);
		m_CancelBinding = &m_InputManager.GetBinding(TrackToolAction::Cancel);
		SetEnable(true);
	}
	void BulldozeToolSystem::OnUpdate()
	{
		if (UIManager::Instance().IsMouseOverUI())
		{
			Cleanup();
			return;
		}

		if (m_ApplyBinding->justPressed)
		{
			Apply();
			return;
		}
		else if (m_CancelBinding->justPressed)
		{
			Cancel();
			return;
		}

		Update();
	}
	void BulldozeToolSystem::OnDestroy()
	{
		Cleanup();
	}
	void BulldozeToolSystem::OnEnabled(bool enabled)
	{
		m_InputManager.EnableBlock<TrackToolAction>(enabled);
		Cleanup();
	}
	void BulldozeToolSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
	}
	void BulldozeToolSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
	}
	void BulldozeToolSystem::Update()
	{
		if (m_State == State::Default)
		{
			TrackObject* result = RaycastUtils::PerformRaycast<TrackObject>(*m_World, GameWindow::Instance().Get());

			if (result == m_EndTrackObject)
				return;

			if (!result || !result->m_CanBeDeleted)
			{
				if (m_EndTrackObject) m_EndTrackObject->ResetColor();
				m_EndTrackObject = nullptr;
				return;
			}

			if (m_EndTrackObject) m_EndTrackObject->ResetColor();
			m_EndTrackObject = result;
			m_EndTrackObject->SetColor(kBulldozeColorHover);

			return;
		}

		TrackObject* result = RaycastUtils::PerformRaycast<TrackObject>(*m_World, GameWindow::Instance().Get());
		if (!result || !result->m_CanBeDeleted)
			m_EndTrackObject = m_StartTrackObject;
		else if (result != m_EndTrackObject)
			m_EndTrackObject = result;
		else
			return;

		if (!m_StartTrackObject || !m_EndTrackObject)
			return;

		for (auto& track : m_TrackPath)
		{
			track->ResetColor();
		}
		m_TrackPath = TrackUtils::GetPath(m_StartTrackObject, m_EndTrackObject);
		m_Logger.InfoO("TrackPath lenght ", m_TrackPath.size());

		for (auto& track : m_TrackPath)
		{
			track->SetColor(kBulldozeColor);
		}
	}
	void BulldozeToolSystem::Apply()
	{
		if (m_State == State::Default)
		{
			TrackObject* result = RaycastUtils::PerformRaycast<TrackObject>(*m_World, GameWindow::Instance().Get());

			if (!result || !result->m_CanBeDeleted)
			{
				return;
			}

			m_StartTrackObject = result;
			if (m_EndTrackObject)
				m_EndTrackObject->ResetColor();

			m_EndTrackObject = nullptr;

			SetState(State::Destroying);
		}
		else if (m_State == State::Destroying)
		{
			TrackObject* result = RaycastUtils::PerformRaycast<TrackObject>(*m_World, GameWindow::Instance().Get());

			if (!result || !result->m_CanBeDeleted)
			{
				return;
			}

			m_Logger.Info("Hello");
			for (auto& track : m_TrackPath)
			{
				m_Logger.Info("Hello2");
				m_World->DestroyObject(track);
			}
			m_StartTrackObject = nullptr;
			m_EndTrackObject = nullptr;
			m_TrackPath.clear();
			SetState(State::Default);
		}
	}
	void BulldozeToolSystem::Cancel()
	{
		if (m_State == State::Default)
		{
			Cleanup();
			SetEnable(false);
			m_DefaultToolSystem->SetEnable(true);
			return;
		}
		else if (m_State == State::Destroying)
		{
			Cleanup();
			SetState(State::Default);
		}
	
	}
	void BulldozeToolSystem::Cleanup()
	{
		for (auto& track : m_TrackPath)
		{
			track->ResetColor();
		}
		m_TrackPath.clear();

		if (m_EndTrackObject) m_EndTrackObject->ResetColor();
		if (m_StartTrackObject) m_StartTrackObject->ResetColor();
		m_StartTrackObject = nullptr;
		m_EndTrackObject = nullptr;
		SetState(State::Default);
	}
	void BulldozeToolSystem::SetState(State state)
	{
		m_State = state;
	}
}