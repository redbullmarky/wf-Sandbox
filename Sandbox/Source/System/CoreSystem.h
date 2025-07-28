#pragma once
#include "Engine.h"

namespace Sandbox
{
	class CoreSystem : public wf::ISystem
	{
	public:
		CoreSystem(wf::Scene* scene);
		~CoreSystem() = default;

		virtual bool init() override;
		virtual void update(float dt) override;
		virtual void shutdown() override;
	};
}