#pragma once
#include "Engine.h"

namespace Sandbox
{
	class CameraSystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;
	};
}