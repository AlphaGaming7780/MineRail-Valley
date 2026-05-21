#include <Game/Systems/SystemBase.hpp>


namespace Game
{
	void SystemBase::SetEnable(bool enable)
	{
		if (m_Enabled == enable) return;
		m_Enabled = enable;
		OnEnabled(enable);
	}
	bool SystemBase::IsEnabled() const
	{
		return m_Enabled;
	}
	void SystemBase::OnEvent(const LoadingStart& ev)
	{
		OnGameLoadingStart(ev.m_GameMode, ev.m_Purpose, ev.m_MapData);
	}

	void SystemBase::OnEvent(const LoadingComplete& ev)
	{
		OnGameLoadingComplete(ev.m_GameMode, ev.m_Purpose, ev.m_MapData);
	}

}