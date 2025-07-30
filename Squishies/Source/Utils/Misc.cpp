#include "Misc.h"

namespace Squishies
{
	std::vector<wf::Vec3> transformPoints(
		const std::vector<wf::Vec3>& originalPoints,
		const wf::Vec3& derivedPosition,
		const wf::Quat& derivedRotation
	) {
		std::vector<wf::Vec3> transformed;
		transformed.reserve(originalPoints.size());

		for (const auto& p : originalPoints)
			transformed.push_back(derivedPosition + derivedRotation * p);

		return transformed;
	}

	std::vector<wf::Vec3> createCircleShape(float radius, size_t segments)
	{
		std::vector<wf::Vec3> points;
		points.resize(segments);

		for (size_t i = 0; i < segments; i++) {
			float angle = (2.f * PI * i) / segments;
			float x = radius * cos(angle);
			float y = radius * sin(angle);

			points[i] = { x, y, 0.f };
		}

		return points;
	}

	std::shared_ptr<wf::Mesh> createCircleMeshFromPoints(const std::vector<wf::Vec3>& edgePoints)
	{
		auto mesh = wf::Mesh::create();

		size_t segments = edgePoints.size();
		mesh->vertices.resize(segments + 1); // +1 for center

		// center vertex
		mesh->vertices[0].position = { 0.f, 0.f, 0.f };
		mesh->vertices[0].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[0].colour = wf::WHITE;
		mesh->vertices[0].texcoord = { 0.5f, 0.5f };

		// compute max extent for texcoord normalisation
		float maxRadius = 0.f;
		for (const auto& p : edgePoints)
			maxRadius = std::max(maxRadius, glm::length(glm::vec2(p)));

		for (size_t i = 0; i < segments; i++) {
			const auto& pos = edgePoints[i];
			mesh->vertices[i + 1].position = pos;
			mesh->vertices[i + 1].normal = { 0.f, 0.f, 1.f };
			mesh->vertices[i + 1].colour = wf::WHITE;

			glm::vec2 tex = glm::vec2(pos.x, pos.y) / maxRadius;
			tex = tex * 0.5f + 0.5f;
			mesh->vertices[i + 1].texcoord = tex;
		}

		mesh->indices.reserve(segments * 3);
		for (unsigned int i = 0; i < segments; i++) {
			mesh->indices.push_back(0);
			mesh->indices.push_back(i + 1);
			mesh->indices.push_back((i + 1) % segments + 1);
		}

		// final triangle to close the loop
		mesh->indices.push_back(0);
		mesh->indices.push_back((unsigned int)segments);
		mesh->indices.push_back(1);

		int c = 0;
		for (auto& i : mesh->indices) {
			printf("%u\n", i);
			c++;
			if (c == 3) {
				printf("---\n");
				c = 0;
			}
		}

		wf::mesh::generateMeshTangents(mesh.get());
		return mesh;
	}
}