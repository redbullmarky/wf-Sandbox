#pragma once
#include "Geometry/Mesh.h"

#include <memory>

namespace wf::component
{
	struct Geometry
	{
		std::shared_ptr<Mesh> mesh;					// the actual mesh geometry
	};
}