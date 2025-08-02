#pragma once
#include "Geometry/Mesh.h"
#include "Render/Material.h"

#include <memory>

namespace wf
{
	struct MeshRendererComponent
	{
		std::shared_ptr<Mesh> mesh;		// mesh to render
		Material material;				// primary material to use
	};
}