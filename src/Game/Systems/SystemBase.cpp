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
}