#include<Game/Systems/TrackToolSystem.h>
#include<Game/InputsActionSets/TrackToolInputSet.h>


namespace Game
{
	void TrackToolSystem::OnCreate()
	{
		SystemBase::OnCreate();

		m_DefaultToolSystem = m_World->GetOrCreateSystem<DefaultToolSystem>();

		m_InputManager.Register<TrackInputs>();
		m_ApplyBinding = &m_InputManager.GetBinding(TrackToolAction::Apply);
		m_CancleBinding = &m_InputManager.GetBinding(TrackToolAction::Cancle);
	}

	void TrackToolSystem::Update()
	{
		if (m_State == State::Default)
		{
			if (m_ApplyBinding->justPressed)
			{

			} 
			else if (m_CancleBinding->justPressed)
			{
				SetEnable(false);
				m_DefaultToolSystem->SetEnable(true);
			}
		}
		else
		{

		}
	}

	void TrackToolSystem::OnDestroy()
	{

	}

	void TrackToolSystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
	{
		SetEnable(false);
	}

	void TrackToolSystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
	{

	}
}