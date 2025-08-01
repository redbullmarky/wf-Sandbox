#pragma once
#include "Geometry/Mesh.h"

#include <memory>

namespace wf
{
	struct GeometryComponent
	{
		std::shared_ptr<Mesh> mesh;					// the actual mesh geometry
	};
}