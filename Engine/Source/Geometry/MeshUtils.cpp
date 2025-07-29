#include "pch.h"
#include "MeshUtils.h"

#include "Geometry/Geometry.h"
#include "Geometry/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace wf::mesh
{
	void generateMeshTangents(Mesh* mesh)
	{
		if (!mesh || mesh->vertices.empty() || mesh->indices.empty()) return;

		size_t vertexCount = mesh->vertices.size();

		std::vector<Vec3> tan1(vertexCount, Vec3(0));
		std::vector<Vec3> tan2(vertexCount, Vec3(0));

		size_t triangleCount = mesh->indices.size() / 3;

		for (size_t i = 0; i < triangleCount; ++i) {
			unsigned int i0 = mesh->indices[i * 3 + 0];
			unsigned int i1 = mesh->indices[i * 3 + 1];
			unsigned int i2 = mesh->indices[i * 3 + 2];

			auto& v0 = mesh->vertices[i0];
			auto& v1 = mesh->vertices[i1];
			auto& v2 = mesh->vertices[i2];

			Vec3 p0 = v0.position;
			Vec3 p1 = v1.position;
			Vec3 p2 = v2.position;

			Vec2 uv0 = v0.texcoord;
			Vec2 uv1 = v1.texcoord;
			Vec2 uv2 = v2.texcoord;

			Vec3 deltaPos1 = p1 - p0;
			Vec3 deltaPos2 = p2 - p0;

			Vec2 deltaUV1 = uv1 - uv0;
			Vec2 deltaUV2 = uv2 - uv0;

			float r = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			if (r != 0.0f) r = 1.0f / r;

			Vec3 sdir = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			Vec3 tdir = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			tan1[i0] += sdir;
			tan1[i1] += sdir;
			tan1[i2] += sdir;

			tan2[i0] += tdir;
			tan2[i1] += tdir;
			tan2[i2] += tdir;
		}

		for (size_t i = 0; i < vertexCount; ++i) {
			Vec3 n = mesh->vertices[i].normal;
			Vec3 t = tan1[i];

			// Gram-Schmidt orthogonalise
			Vec3 tangent = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness (w component)
			float w = (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

			mesh->vertices[i].tangent = glm::vec4(tangent, w);
		}
	}
}