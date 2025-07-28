#pragma once
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"

#include <vector>

namespace wf::wgl
{
	struct MeshBuffers
	{
		unsigned int vao{};
		unsigned int vbo{};
		unsigned int ebo{};
	};

	bool init();

	void setViewport(int w, int h);
	void setViewport(int x, int y, int w, int h);

	void enableDepthTest(bool enable = true);

	void clear(bool colour = true, bool depth = true);
	void clearColour(const Colour& colour, bool clearDepth = false);

	[[nodiscard]] MeshBuffers createMeshBuffers();
	void destroyMeshBuffers(MeshBuffers& buffers);
	void uploadMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		bool dynamic = false
	);
	void updateMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices
	);
	void drawMeshBuffers(const MeshBuffers& buffers, int vertexCount, int indexCount, bool wireframe = false);

}