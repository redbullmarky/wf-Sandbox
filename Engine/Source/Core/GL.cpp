#include "pch.h"
#include "GL.h"

#include "Geometry/Geometry.h"

#include "GL/glew.h"

namespace wf::wgl
{
	bool init()
	{
		GLenum result = glewInit();
		return result == GLEW_OK;
	}

	void setViewport(int w, int h)
	{
		setViewport(0, 0, w, h);
	}

	void setViewport(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}

	void enableDepthTest(bool enable)
	{
		if (enable) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	void clear(bool colour, bool depth)
	{
		GLbitfield flags{};
		if (colour) flags |= GL_COLOR_BUFFER_BIT;
		if (depth) flags |= GL_DEPTH_BUFFER_BIT;

		glClear(flags);
	}

	void clearColour(const Colour& colour, bool clearDepth)
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);

		clear(true, clearDepth);
	}

	[[nodiscard]] MeshBuffers createMeshBuffers()
	{
		MeshBuffers buffers;

		glGenVertexArrays(1, &buffers.vao);
		glGenBuffers(1, &buffers.vbo);
		glGenBuffers(1, &buffers.ebo);

		glBindVertexArray(buffers.vao);
		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		glBindVertexArray(0);
		return buffers;
	}

	void destroyMeshBuffers(MeshBuffers& buffers)
	{
		if (buffers.vao) glDeleteVertexArrays(1, &buffers.vao);
		if (buffers.vbo) glDeleteBuffers(1, &buffers.vbo);
		if (buffers.ebo) glDeleteBuffers(1, &buffers.ebo);
		buffers = {};
	}

	void uploadMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		bool dynamic
	) {
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot upload to non-initialised buffers");
		if (!buffers.ebo && indices.size()) throw std::runtime_error("Indices provided but no ebo created");

		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if (indices.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
	}

	void updateMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices
	) {
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot update non-initialised buffers");
		if (!buffers.ebo && indices.size()) throw std::runtime_error("Indices provided but no ebo created");

		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		if (indices.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());
		}
	}

	void drawMeshBuffers(const MeshBuffers& buffers, int vertexCount, int indexCount, bool wireframe)
	{
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot draw non-initialised buffers");
		if (!buffers.ebo && indexCount) throw std::runtime_error("Indices provided but no ebo created");

		glBindVertexArray(buffers.vao);

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (buffers.ebo)
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(0);
	}
}
