#pragma once
#include "Engine.h"

// stuff in here will likely get shifted to the core scene, eventually

namespace Sandbox
{
	class BaseScene : public wf::Scene
	{
	public:
		BaseScene() = default;
		~BaseScene() = default;

		virtual bool init() override;
	};
}