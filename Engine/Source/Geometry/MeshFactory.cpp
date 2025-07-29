#include "pch.h"
#include "MeshFactory.h"

#include "Math/Math.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "Misc/Colour.h"

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

namespace wf::mesh
{
	std::shared_ptr<Mesh> createSimplePlane()
	{
		auto mesh = Mesh::create();

		mesh->vertices.resize(4);

		float w = 250.f;

		mesh->vertices[0].position = Vec3{ -w, 0.f, -w };
		mesh->vertices[1].position = Vec3{ w, 0.f, -w };
		mesh->vertices[2].position = Vec3{ w, 0.f,  w };
		mesh->vertices[3].position = Vec3{ -w, 0.f,  w };

		mesh->vertices[0].texcoord = { 0.f, 0.f };
		mesh->vertices[1].texcoord = { 10.f, 0.f };
		mesh->vertices[2].texcoord = { 10.f, 10.f };
		mesh->vertices[3].texcoord = { 0.f, 10.f };

		Vec3 normal = Vec3{ 0.f, 1.f, 0.f };
		for (auto& v : mesh->vertices)
			v.normal = normal;

		mesh->indices = { 0, 1, 2, 2, 3, 0 };

		return mesh;
	}

	std::shared_ptr<Mesh> createCube(const Vec3& dimensions, const Colour& colour)
	{
		auto mesh = Mesh::create();

		Vec3 min = { -dimensions.x * .5f, -dimensions.y * .5f, -dimensions.y * .5f };
		Vec3 max = { dimensions.x * .5f, dimensions.y * .5f, dimensions.y * .5f };

		mesh->vertices.resize(8);
		mesh->vertices[0].position = { min.x, min.y, min.z };
		mesh->vertices[1].position = { max.x, min.y, min.z };
		mesh->vertices[2].position = { max.x, max.y, min.z };
		mesh->vertices[3].position = { min.x, max.y, min.z };
		mesh->vertices[4].position = { min.x, min.y, max.z };
		mesh->vertices[5].position = { max.x, min.y, max.z };
		mesh->vertices[6].position = { max.x, max.y, max.z };
		mesh->vertices[7].position = { min.x, max.y, max.z };

		mesh->vertices[0].colour = colour;
		mesh->vertices[1].colour = colour;
		mesh->vertices[2].colour = colour;
		mesh->vertices[3].colour = colour;
		mesh->vertices[4].colour = colour;
		mesh->vertices[5].colour = colour;
		mesh->vertices[6].colour = colour;
		mesh->vertices[7].colour = colour;

		mesh->vertices[0].normal = { -1.f, -1.f, -1.f };
		mesh->vertices[1].normal = { 1.f, -1.f, -1.f };
		mesh->vertices[2].normal = { 1.f,  1.f, -1.f };
		mesh->vertices[3].normal = { -1.f,  1.f, -1.f };
		mesh->vertices[4].normal = { -1.f, -1.f,  1.f };
		mesh->vertices[5].normal = { 1.f, -1.f,  1.f };
		mesh->vertices[6].normal = { 1.f,  1.f,  1.f };
		mesh->vertices[7].normal = { -1.f,  1.f,  1.f };

		mesh->vertices[0].texcoord = { 0.f, 0.f };
		mesh->vertices[1].texcoord = { 1.f, 0.f };
		mesh->vertices[2].texcoord = { 1.f, 1.f };
		mesh->vertices[3].texcoord = { 0.f, 1.f };
		mesh->vertices[4].texcoord = { 0.f, 0.f };
		mesh->vertices[5].texcoord = { 1.f, 0.f };
		mesh->vertices[6].texcoord = { 1.f, 1.f };
		mesh->vertices[7].texcoord = { 0.f, 1.f };

		mesh->indices = {
			4, 5, 6, // front
			4, 6, 7,
			1, 0, 3, // back
			1, 3, 2,
			0, 4, 7, // left
			0, 7, 3,
			5, 1, 2, // right
			5, 2, 6,
			3, 7, 6, // top
			3, 6, 2,
			0, 1, 5, // bottom
			0, 5, 4
		};

		generateMeshTangents(mesh.get());

		return mesh;
	}

	std::shared_ptr<Mesh> createCubeExt(const Vec3& dimensions, const Colour& colour)
	{
		auto mesh = Mesh::create();

		Vec3 min = { -dimensions.x * .5f, -dimensions.y * .5f, -dimensions.y * .5f };
		Vec3 max = { dimensions.x * .5f, dimensions.y * .5f, dimensions.y * .5f };

		mesh->vertices.resize(24);

		int vertIndex{ 0 };

		// FRONT
		mesh->vertices[vertIndex].position = { min.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { min.y, max.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, 1.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		// BACK
		mesh->vertices[vertIndex].position = { max.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, -1.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { min.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, -1.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { min.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, -1.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 0.f, -1.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		// LEFT
		mesh->vertices[vertIndex].position = { min.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { -1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { min.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { -1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { min.x, max.y, max.z };
		mesh->vertices[vertIndex].normal = { -1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { min.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { -1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		// RIGHT
		mesh->vertices[vertIndex].position = { max.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { 1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { 1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { 1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, max.z };
		mesh->vertices[vertIndex].normal = { 1.f, 0.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		// TOP
		mesh->vertices[vertIndex].position = { min.x, max.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, 1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { min.x, max.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, 1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		// BOTTOM
		mesh->vertices[vertIndex].position = { min.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, -1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, min.y, min.z };
		mesh->vertices[vertIndex].normal = { 0.f, -1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 0.f };
		mesh->vertices[vertIndex].position = { max.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, -1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 1.f, 1.f };
		mesh->vertices[vertIndex].position = { min.x, min.y, max.z };
		mesh->vertices[vertIndex].normal = { 0.f, -1.f, 0.f };
		mesh->vertices[vertIndex++].texcoord = { 0.f, 1.f };

		mesh->indices = {
			0, 1, 2, 0, 2, 3, // FRONT
			4, 5, 6, 4, 6, 7, // BACK
			8, 9,10, 8,10,11, // LEFT
			12,13,14,12,14,15, // RIGHT
			16,17,18,16,18,19, // TOP
			20,21,22,20,22,23 // BOTTOM
		};

		for (int i = 0; i < 24; i++) {
			mesh->vertices[i].colour = colour;
		}

		generateMeshTangents(mesh.get());

		return mesh;
	}

	std::shared_ptr<Mesh> createSphere(float radius, int rings, int slices)
	{
		auto mesh = Mesh::create();

		mesh->vertices.reserve(static_cast<size_t>((rings + 1) * (slices + 1)));

		if (rings < 3) rings = 3;
		if (slices < 3) slices = 3;

		for (int r = 0; r <= rings; r++) {
			float v = (float)r / (float)rings;
			float phi = v * PI; // polar angle

			for (int s = 0; s <= slices; s++) {
				float u = (float)s / (float)slices;
				float theta = u * PI * 2.f; // azimuthal angle

				float x = radius * sinf(phi) * sinf(theta);
				float y = radius * cosf(phi);
				float z = -(radius * sinf(phi) * cosf(theta));

				Vec3 position = { x, y, z };
				Vec3 normal = glm::normalize(position);
				Vec2 texcoord = { 1.f - u, 1.f - v };

				Vertex vert;
				vert.position = position;
				vert.normal = normal;
				vert.texcoord = texcoord;
				mesh->vertices.push_back(vert);
			}
		}

		mesh->indices.reserve(static_cast<size_t>(rings * slices * 6));

		for (int r = 0; r < rings; r++) {
			for (int s = 0; s < slices; s++) {
				int i0 = r * (slices + 1) + s;
				int i1 = i0 + slices + 1;

				mesh->indices.push_back(i0);
				mesh->indices.push_back(i0 + 1);
				mesh->indices.push_back(i1);

				mesh->indices.push_back(i1);
				mesh->indices.push_back(i0 + 1);
				mesh->indices.push_back(i1 + 1);
			}
		}

		generateMeshTangents(mesh.get());

		return mesh;
	}

	std::shared_ptr<Mesh> createHelloTriangle()
	{
		auto mesh = Mesh::create();

		mesh->vertices.resize(3);
		mesh->vertices[0].position = Vec3{ -0.5f, -0.5f, 0.0f, };
		mesh->vertices[1].position = Vec3{ 0.5f, -0.5f, 0.0f, };
		mesh->vertices[2].position = Vec3{ 0.0f,  0.5f, 0.0f };

		mesh->vertices[0].normal = Vec3{ 0.f, 0.f, 1.f };
		mesh->vertices[1].normal = Vec3{ 0.f, 0.f, 1.f };
		mesh->vertices[2].normal = Vec3{ 0.f, 0.f, 1.f };

		mesh->vertices[0].colour = WHITE; //{ 1.f, 0.f, 0.f, 1.f };
		mesh->vertices[1].colour = WHITE; //{ 0.f, 1.f, 0.f, 1.f };
		mesh->vertices[2].colour = WHITE; //{ 0.f, 0.f, 1.f, 1.f };

		mesh->vertices[0].texcoord = { 0.f, 0.f };
		mesh->vertices[1].texcoord = { 1.f, 0.f };
		mesh->vertices[2].texcoord = { 0.5f, 1.f };

		mesh->indices = { 0, 1, 2 };

		generateMeshTangents(mesh.get());

		return mesh;
	}

	std::shared_ptr<Mesh> createSphere2(float radius, int rings, int slices)
	{
		auto mesh = Mesh::create();

		par_shapes_set_epsilon_degenerate_sphere(0.f);
		par_shapes_mesh* sphere = par_shapes_create_parametric_sphere(slices, rings);
		par_shapes_scale(sphere, radius, radius, radius);

		mesh->vertices.resize(static_cast<size_t>(sphere->npoints));
		mesh->indices.resize(static_cast<size_t>(sphere->ntriangles * 3));

		for (int i = 0; i < sphere->npoints; i++) {
			Vertex v{};
			v.position = {
				sphere->points[i * 3],
				sphere->points[i * 3 + 1],
				sphere->points[i * 3 + 2]
			};
			v.normal = {
				sphere->normals[i * 3],
				sphere->normals[i * 3 + 1],
				sphere->normals[i * 3 + 2]
			};
			v.texcoord = {
				sphere->tcoords[i * 2],
				sphere->tcoords[i * 2 + 1]
			};
			mesh->vertices[i] = v;
		}

		// Copy indices directly
		for (int i = 0; i < sphere->ntriangles * 3; i++)
			mesh->indices[i] = sphere->triangles[i];

		par_shapes_free_mesh(sphere);

		generateMeshTangents(mesh.get());

		return mesh;
	}
}