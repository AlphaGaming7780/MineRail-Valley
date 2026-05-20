#include<Game/Systems/TrackToolSystem.h>
#include<Game/InputsActionSets/TrackToolInputSet.h>
#include <Game/Utils/RaycastUtils.h>


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

	void TrackToolSystem::Update()
	{
		if (m_ApplyBinding->justPressed)
		{
			Apply();
		}
		else if (m_CancelBinding->justPressed)
		{
			Cancel();
		}
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

	void TrackToolSystem::Apply()
	{
		if (m_State == State::Default)
		{
			GameObject* result = RaycastUtils::PerformRaycast(*m_World, GameWindow::Instance().Get());
			
			if (!result)
			{
				m_Logger.WarnO("result is null");
				return;
			}

			

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



	}
}