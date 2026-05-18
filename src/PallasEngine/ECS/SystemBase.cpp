#include <PallasEngine/ECS/Systems/SystemBase.hpp>


namespace pallas
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
	entt::entity SystemBase::CreateEntity()
	{
		return m_Registry->create();
	}
}