#pragma once
#include "Engine.h"

#include <memory>

namespace Sandbox::Component
{
	struct Geometry
	{
		std::shared_ptr<wf::Mesh> mesh;					// the actual mesh geometry
	};
}