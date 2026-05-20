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
		m_InputManager.EnableBlock<TrackToolAction>(enabled);
	}

	void TrackToolSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
	}

	void TrackToolSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
	}

	void TrackToolSystem::SetState(State state)
	{
		m_State = state;
	}

	void TrackToolSystem::Update()
	{
		if (m_State == State::Default)
		{
			TileObject* result = RaycastUtils::PerformRaycast<TileObject>(*m_World, GameWindow::Instance().Get());

			if (result == m_EndTileObject)
				return;

			if (!result || !result->m_CanBuild)
			{
				if(m_EndTileObject) m_EndTileObject->ResetColor();
				m_EndTileObject = nullptr;
				return;
			}

			if (m_EndTileObject) m_EndTileObject->ResetColor();
			m_EndTileObject = result;
			m_EndTileObject->SetColor(DefaultToolSystem::kHoverColor);

			return;
		}

		TileObject* result = RaycastUtils::PerformRaycast<TileObject>(*m_World, GameWindow::Instance().Get());
		if (!result || !result->m_CanBuild)
			m_EndTileObject = m_StartTileObject;
		else if (result != m_EndTileObject)
			m_EndTileObject = result;
		else
			return;

		if (!m_StartTileObject || !m_EndTileObject)
			return;

		// Recalcule le chemin complet
		m_TilePath = TerrainUtils::GetPath(*m_World, m_StartTileObject, m_EndTileObject);

		std::vector<TileObject*> filteredTiles;
		filteredTiles.reserve(m_TilePath.size());

		for (TileObject* tile : m_TilePath)
		{
			// Si un track permanent existe -> on ne crée PAS de track temporaire
			if (tile->m_PlacedTrack && !tile->m_PlacedTrack->m_Temp)
				continue;

			filteredTiles.push_back(tile);
		}

		size_t needed = filteredTiles.size();
		size_t existing = m_TrackPath.size();

		if (existing > needed)
		{
			for (size_t i = needed; i < existing; ++i)
				m_World->DestroyObject(m_TrackPath[i]);

			m_TrackPath.resize(needed);
		}

		if (existing < needed)
		{
			for (size_t i = existing; i < needed; ++i)
			{
				const TrackData& t = *TrackDatabase::Instance().Load("Tracks\\BaseTrack.json");
				TrackObject* track = m_World->CreateGameObject<TrackObject>(t);
				track->m_Temp = true;
				m_TrackPath.push_back(track);
			}
		}

		for (size_t i = 0; i < needed; ++i)
		{
			TrackObject* track = m_TrackPath[i];
			TileObject* tile = filteredTiles[i];

			track->SetTile(tile);
			track->SetColor(kBuildColor);
			if (i > 0) track->m_First = m_TrackPath[i - 1];
			else track->m_First == nullptr;
			if (i + 1 < needed) track->m_Second = m_TrackPath[i + 1];
			else track->m_Second = nullptr;
			track->m_Enabled = true;
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
			if (m_EndTileObject)
				m_EndTileObject->ResetColor();
				
			m_EndTileObject = nullptr;

			SetState(State::Creating);

		}
		else if (m_State == State::Creating)
		{
			TileObject* result = RaycastUtils::PerformRaycast<TileObject>(*m_World, GameWindow::Instance().Get());

			if (!result || !result->m_CanBuild)
			{
				return;
			}

			//Play sound ?

			for (TrackObject* to : m_TrackPath)
			{
				to->m_Temp = false;
				to->m_Enabled = true;
				to->ResetColor();
			}

			m_TrackPath.clear();
			m_TilePath.clear();

			SetState(State::Default);
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
			for (TrackObject* td : m_TrackPath)
			{
				m_World->DestroyObject(td);
			}

			m_TilePath.clear();
			m_TrackPath.clear();
			SetState(State::Default);
		}
	}
}