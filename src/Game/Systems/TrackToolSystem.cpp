#include<Game/Systems/TrackToolSystem.h>
#include<Game/InputsActionSets/TrackToolInputSet.h>
#include <Game/Utils/RaycastUtils.h>
#include <Game/Utils/TerrainUtils.h>
#include <Game/AssetDatabase/TrackDatabase.h>

namespace Game
{
	void TrackToolSystem::OnCreate()
	{
		SystemBase::OnCreate();

		m_DefaultToolSystem = m_World->GetOrCreateSystem<DefaultToolSystem>();

		m_InputManager.Register<TrackInputs>();
		m_ApplyBinding = &m_InputManager.GetBinding(TrackToolAction::Apply);
		m_CancelBinding = &m_InputManager.GetBinding(TrackToolAction::Cancle);
		SetEnable(true);
	}

	void TrackToolSystem::OnUpdate()
	{
		if (m_ApplyBinding->justPressed)
		{
			Apply();
		}
		else if (m_CancelBinding->justPressed)
		{
			Cancel();
		}

		Update();
	}

	void TrackToolSystem::OnDestroy()
	{

	}

	void TrackToolSystem::OnEnabled(bool enabled)
	{
		m_Logger.InfoO("TrackToolSystem ", (enabled ? "enabled" : "disabled"));
		m_InputManager.EnableBlock<TrackToolAction>(enabled);
	}

	void TrackToolSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		m_Logger.InfoO("TrackToolSystem received OnGameLoadingStart, GameMode: ", magic_enum::enum_name(mode), " | Purpose: ", magic_enum::enum_name(purpose));
		SetEnable(false);
	}

	void TrackToolSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
	{
		m_Logger.InfoO("TrackToolSystem received OnGameLoadingStart, GameMode: ", magic_enum::enum_name(mode), " | Purpose: ", magic_enum::enum_name(purpose));
		SetEnable(true);
	}

	void TrackToolSystem::SetState(State state)
	{
		m_State = state;
	}

	void TrackToolSystem::Update()
	{
		if (m_State == State::Creating)
		{
			TileObject* result = RaycastUtils::PerformRaycast<TileObject>(*m_World, GameWindow::Instance().Get());

			if (result == m_EndTileObject)
			{
				return;
			}

			//TODO : PlaySound;

			m_EndTileObject = result;

			if (!m_StartTileObject || !m_EndTileObject)
				return;

			for (TileObject* to : m_TilePath)
			{
				to->ResetColor();
			}

			m_TilePath.clear();

			m_TilePath = TerrainUtils::GetPath(*m_World, m_StartTileObject, m_EndTileObject);

			for (TileObject* to : m_TilePath)
			{
				TrackObject* track = m_World->CreateGameObject<TrackObject>(TrackDatabase::Instance().GetDefault());
				track->SetTile(to);
				track->SetColor(sf::Color::Cyan);
			}
		}
	}

	void TrackToolSystem::Apply()
	{
		if (m_State == State::Default)
		{
			TileObject* result = RaycastUtils::PerformRaycast<TileObject>(*m_World, GameWindow::Instance().Get());
			
			if (!result || !result->m_CanBuild)
			{
				return;
			}

			//Play sound ?

			m_StartTileObject = result;

			SetState(State::Creating);

			m_Logger.InfoO("Selected tile: ", m_StartTileObject->m_Index);

		}
		else if (m_State == State::Creating)
		{
		}

	}

	void TrackToolSystem::Cancel()
	{

		if (m_State == State::Default)
		{
			//SetEnable(false);
			//m_DefaultToolSystem->SetEnable(true);
			return;
		}
		else if (m_State == State::Creating)
		{
			for (TileObject* to : m_TilePath)
			{
				to->ResetColor();
			}
			m_TilePath.clear();
			SetState(State::Default);
		}
	}
}