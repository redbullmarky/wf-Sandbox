#include "pch.h"

#include "Geometry/Mesh.h"
#include "Math/PerlinNoise.h"
#include "Math/SimplexNoise.h"
#include "MeshUtils.h"
#include "Terrain.h"

#include <memory>

namespace wf::mesh
{
	std::shared_ptr<Mesh> Terrain::createPlane(int width, int height, int resolution, float tileFactor)
	{
		auto mesh = Mesh::create();

		int vertCountX = resolution + 1;
		int vertCountZ = resolution + 1;
		size_t vertexCount = (size_t)vertCountX * (size_t)vertCountZ;
		size_t triCount = (size_t)resolution * (size_t)resolution * 2;

		float dx = (float)width / resolution;
		float dz = (float)height / resolution;

		mesh->vertices.resize(vertexCount);
		mesh->indices.resize(triCount * 3);

		for (int vIndex = 0, z = 0; z < vertCountZ; z++) {
			for (int x = 0; x < vertCountX; x++) {
				float worldX = x * dx;
				float worldZ = z * dz;

				Vertex v{};
				v.position = { worldX, 0.f, worldZ };
				v.normal = { 0.f, 1.f, 0.f };
				v.texcoord = { ((float)x / resolution) * tileFactor, ((float)z / resolution) * tileFactor };

				mesh->vertices[vIndex] = v;
				vIndex++;
			}
		}

		for (int tIndex = 0, z = 0; z < resolution; z++) {
			for (int x = 0; x < resolution; x++) {
				int i0 = z * vertCountX + x;
				int i1 = i0 + 1;
				int i2 = i0 + vertCountX;
				int i3 = i2 + 1;

				mesh->indices[tIndex++] = i0;
				mesh->indices[tIndex++] = i2;
				mesh->indices[tIndex++] = i1;

				mesh->indices[tIndex++] = i1;
				mesh->indices[tIndex++] = i2;
				mesh->indices[tIndex++] = i3;
			}
		}

		return mesh;
	}

	void Terrain::applyCenter(std::shared_ptr<Mesh> mesh, const Vec2& size)
	{
		if (!mesh->vertices.size()) return;

		float offsetX = size.x * 0.5f;
		float offsetZ = size.y * 0.5f;

		for (auto& vert : mesh->vertices) {
			vert.position.x -= offsetX;
			vert.position.z -= offsetZ;
		}
	}

	void Terrain::applyMinY(std::shared_ptr<Mesh> mesh, float y)
	{
		if (!mesh->vertices.size()) return;

		float minY = std::numeric_limits<float>::max();

		for (auto& vert : mesh->vertices) {
			if (vert.position.y < minY) {
				minY = vert.position.y;
			}
		}

		float offset = y - minY;

		for (auto& vert : mesh->vertices) {
			vert.position.y += offset;
		}
	}

	void Terrain::applyAdjustY(std::shared_ptr<Mesh> mesh, float yDiff)
	{
		if (!mesh->vertices.size()) return;

		for (auto& vert : mesh->vertices) {
			vert.position.y += yDiff;
		}
	}

	void Terrain::applyPerlinNoise(std::shared_ptr<Mesh> mesh, float frequency, float amplitude, int seed)
	{
		if (!mesh->vertices.size()) return;

		PerlinNoise perlin(seed);

		for (auto& vert : mesh->vertices) {
			vert.position.y = perlin.getValue(vert.position.x * frequency, vert.position.z * frequency) * amplitude;
		}
	}

	void Terrain::applySimplexNoise(std::shared_ptr<Mesh> mesh, float frequency, float amplitude, int seed)
	{
		if (!mesh->vertices.size()) return;

		SimplexNoise simplex(seed);

		for (auto& vert : mesh->vertices) {
			vert.position.y = simplex.getValue(vert.position.x * frequency, vert.position.z * frequency) * amplitude;
		}
	}

	void Terrain::applyFractalSimplexNoise(std::shared_ptr<Mesh> mesh, float baseFreq, float amplitude, int seed, int octaves, float persistence, float lacunarity)
	{
		if (!mesh->vertices.size()) return;

		SimplexNoise simplex(seed);

		for (auto& vert : mesh->vertices) {

			float noise = fractalNoise2D(
				simplex.getValue,
				vert.position.x * baseFreq,
				vert.position.z * baseFreq,
				octaves,
				persistence,
				lacunarity
			);

			vert.position.y = noise * amplitude;
		}
	}

	void Terrain::applyMaskedSimplex(std::shared_ptr<Mesh> mesh, float baseFreq, float amplitude, int seed, int maskSeed, float maskFreq, float maskStrength)
	{
		if (!mesh->vertices.size()) return;

		SimplexNoise simplex(seed);
		SimplexNoise maskSimplex(maskSeed);

		for (auto& vert : mesh->vertices) {

			float heightNoise = fractalNoise2D(
				simplex.getValue,
				vert.position.x * baseFreq,
				vert.position.z * baseFreq,
				4,
				.5f,
				2.f
			);
			float mask = maskSimplex.getValue(vert.position.x * maskFreq, vert.position.z * maskFreq) * .5f + .5f;
			float maskedHeight = heightNoise * (mask * maskStrength);

			vert.position.y = maskedHeight * amplitude;
		}
	}

	void Terrain::applyEdging(std::shared_ptr<Mesh> mesh, Vec2 size, float degree)
	{
		if (!mesh->vertices.size()) return;

		float halfWidth = size.x * .5f;
		float halfDepth = size.y * .5f;

		for (auto& vert : mesh->vertices) {
			// Distance from center
			float dx = fabsf(vert.position.x);
			float dz = fabsf(vert.position.z);

			// How close to edge? (0 = center, 1 = edge)
			float edgeFactorX = dx / halfWidth;
			float edgeFactorZ = dz / halfDepth;

			// Combine edge factors (closer to 1 means closer to edge)
			float edgeFactor = fminf(fmaxf(edgeFactorX, edgeFactorZ), 1.f);

			float drop = powf(edgeFactor, 2.f) * degree;
			vert.position.y -= drop;
		}
	}

	void Terrain::fixTerrainNormalsAndUVsAndStuff(std::shared_ptr<Mesh> mesh, const Vec2& flatDimensions, float tileFactor)
	{
		if (!mesh->vertices.size()) return;

		// Recalculate normals
		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			unsigned int i0 = mesh->indices[i];
			unsigned int i1 = mesh->indices[i + 1];
			unsigned int i2 = mesh->indices[i + 2];

			Vec3 v0 = mesh->vertices[i0].position;
			Vec3 v1 = mesh->vertices[i1].position;
			Vec3 v2 = mesh->vertices[i2].position;

			Vec3 edge1 = v1 - v0;
			Vec3 edge2 = v2 - v0;
			Vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

			for (auto idx : { i0, i1, i2 }) {
				mesh->vertices[idx].normal += faceNormal;
			}
		}

		// Normalise overall normal, and reassign UVs (planar from XZ)
		for (auto& vert : mesh->vertices) {
			vert.normal = glm::normalize(vert.normal);
			vert.texcoord.x = (vert.position.x / flatDimensions.x) * tileFactor;
			vert.texcoord.y = (vert.position.z / flatDimensions.y) * tileFactor;
		}

		generateMeshTangents(mesh.get());
	}

	float Terrain::fractalNoise2D(float(*noiseFunc)(float, float), float x, float y, int octaves, float persistence, float lacunarity)
	{
		float total = 0.0f;
		float amplitude = 1.0f;
		float frequency = 1.0f;
		float maxValue = 0.0f;

		for (int i = 0; i < octaves; i++) {
			total += noiseFunc(x * frequency, y * frequency) * amplitude;
			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= lacunarity;
		}

		return total / maxValue;
	}
}