#pragma once

#include <PallasEngine/ECS/Systems/SystemBase.hpp>

namespace pallas
{
	class CameraSyncOutSystem : public SystemBase
	{
	public:

		CameraSyncOutSystem(World* world, entt::registry* registry, Logger& logger) : SystemBase(world, registry, logger) {}

		virtual UpdatePhase Phase() const override { return UpdatePhase::Rendering; }

		virtual void OnCreate() override;

		virtual void Update() override;

	private:

		entt::entity m_CemeraEntity = entt::null;

	};
}

