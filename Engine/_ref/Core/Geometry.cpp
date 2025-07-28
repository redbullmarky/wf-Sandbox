#include "pch.h"
#include "Geometry.h"

#include <GL/glew.h>

namespace wf
{
	Mesh wf::createMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& indices, bool dynamic)
	{
		Mesh out{};
		out.vertices = verts;
		out.indices = indices;
		out.isDynamic = dynamic;
		uploadMesh(out);
		return out;
	}

	// regular func takes whatever's configured, but this overload will allow 
	void uploadMesh(Mesh& mesh, bool dynamic)
	{
		if (mesh.vbo) throw std::runtime_error("Mesh is already uploaded");
		mesh.isDynamic = dynamic;
		uploadMesh(mesh);
	}

	void uploadMesh(Mesh& mesh)
	{
		glGenVertexArrays(1, &mesh.vao);
		glGenBuffers(1, &mesh.vbo);
		if (mesh.indices.size()) glGenBuffers(1, &mesh.ebo);

		glBindVertexArray(mesh.vao);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), mesh.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if (mesh.indices.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), mesh.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void updateMesh(const Mesh& mesh)
	{
		if (!mesh.isDynamic || !mesh.vbo) throw std::runtime_error("Cannot update static mesh");

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void unloadMesh(Mesh& mesh)
	{
		if (mesh.ebo) {
			glDeleteBuffers(1, &mesh.ebo);
			mesh.ebo = {};
		}
		if (mesh.vbo) {
			glDeleteBuffers(1, &mesh.vbo);
			mesh.vbo = {};
		}

		if (mesh.vao) {
			glDeleteVertexArrays(1, &mesh.vao);
			mesh.vao = {};
		}
	}

	void bindMesh(const Mesh& mesh)
	{
		glBindVertexArray(mesh.vao);
	}

	void unbindMesh(const Mesh& mesh)
	{
		glBindVertexArray(0);
	}
}