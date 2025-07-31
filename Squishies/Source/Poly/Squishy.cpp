#include "Squishy.h"

namespace Squishies
{
	const std::vector<wf::Vec2> Squishy::getPoints() const
	{
		return poly.points;
	}

	const std::vector<Joint> Squishy::getJoints() const
	{
		return joints;
	}

	const wf::Vec2 Squishy::getPoint(size_t index) const
	{
		return poly.points[index];
	}

	std::shared_ptr<wf::Mesh> Squishy::createMesh()
	{
		auto mesh = wf::Mesh::create();

		size_t segments = poly.points.size();
		mesh->vertices.resize(segments + 1); // +1 for center

		// center vertex
		mesh->vertices[0].position = { 0.f, 0.f, 0.f };
		mesh->vertices[0].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[0].colour = wf::WHITE;
		mesh->vertices[0].texcoord = { 0.5f, 0.5f };

		// compute max extent for texcoord normalisation
		float maxRadius = 0.f;
		for (const auto& p : poly.points)
			maxRadius = std::max(maxRadius, glm::length(glm::vec2(p)));

		for (size_t i = 0; i < segments; i++) {
			const auto& pos = poly.points[i];
			mesh->vertices[i + 1].position = wf::Vec3{ pos, 0.f };
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

		wf::mesh::generateMeshTangents(mesh.get());
		return mesh;
	}
}