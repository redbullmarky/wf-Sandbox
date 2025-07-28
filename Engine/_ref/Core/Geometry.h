#pragma once
#include "Math/Math.h"
#include "Mesh/Mesh.h"
#include "Render/Colour.h"
#include "Render/Material.h"

#include <vector>

namespace wf
{
	Mesh createMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices = {}, bool dynamic = false);
	void uploadMesh(Mesh& mesh, bool dynamic);
	void uploadMesh(Mesh& mesh);
	void updateMesh(const Mesh& mesh);
	void unloadMesh(Mesh& mesh);

	void bindMesh(const Mesh& mesh);
	void unbindMesh(const Mesh& mesh);
}