#include <Game/Systems/SystemBase.hpp>


namespace Game
{
	void SystemBase::SetEnable(bool enable)
	{
		m_Enabled = enable;
		OnEnabled(enable);
	}
	bool SystemBase::IsEnabled() const
	{
		return m_Enabled;
	}
	void SystemBase::OnEvent(const LoadingStart& ev)
	{
		OnGameLoadingStart(ev.m_GameMode, ev.m_Purpose);
	}

	void SystemBase::OnEvent(const LoadingComplete& ev)
	{
		OnGameLoadingComplete(ev.m_GameMode, ev.m_Purpose);
	}

}