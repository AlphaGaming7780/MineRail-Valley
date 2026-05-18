#pragma once

#include <PallasEngine/ECS/Systems/SystemBase.hpp>

namespace pallas
{
	class CameraSyncInSystem : public SystemBase
	{
	public:

		CameraSyncInSystem(World* world, entt::registry* registry, Logger& logger) : SystemBase(world, registry, logger) {}

		virtual UpdatePhase Phase() const override { return UpdatePhase::Begin; }

		virtual void OnCreate() override;

		virtual void Update() override;

	private:

		entt::entity m_CemeraEntity = entt::null;

	};
}

