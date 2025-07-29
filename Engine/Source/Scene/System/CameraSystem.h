#pragma once
#include "Scene/System.h"

namespace wf::system
{
	class CameraSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;
	};
}