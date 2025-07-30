#pragma once
#include "Math/Math.h"

#include <memory>

// @todo test, as not been checked since moving over from previous iteration

/**
 * @brief Terrain generation. Mostly test/learning stuff, but somewhat effective...
 * Examples:
 *
 *	TerrainGen::ApplyPerlinNoise(mesh, 0.1f, 5.0f, 42);
 *	TerrainGen::ApplySimplexNoise(mesh, .05, 4.f, 1337);
 *
 * 	// ROLLING HILLS
 *	Engine::TerrainMesh::ApplyFractalSimplexNoise(mesh,
 *		0.01f,
 *		10.0f,
 *		1337,
 *		4,
 *		0.5f,
 *		2.0f
 *	);
 *
 *	// ROCKY HIGHLANDS
 *	Engine::TerrainMesh::ApplyFractalSimplexNoise(mesh,
 *		0.015f,
 *		20.0f,
 *		2025,
 *		6,
 *		0.6f,
 *		2.3f
 *	);
 *
 *  // CRAGGY WASTELAND
 *  Engine::TerrainMesh::ApplyFractalSimplexNoise(mesh,
 *  	0.03f,
 *  	30.0f,
 *  	666,
 *  	5,
 *  	0.75f,
 *  	2.8f
 *  );
 *
 *	// MOUNTAINS IN SPECIFIC REGIONS
 *  Engine::TerrainMesh::ApplyMaskedSimplex(mesh,
 *		0.01f,
 *		40.0f,
 *		4242,
 *		9898,
 *		0.003f,
 *		3.0f
 *	);
 *
 *	// VOLCANO WORLD
 *	Engine::TerrainMesh::ApplyMaskedSimplex(mesh,
 *		0.02f,
 *		50.0f,
 *		777,
 *		777,
 *		0.0015f,
 *		6.0f
 *	);
 *
 *	// ISLANDS & ARCHIPELAGOS
 *	Engine::TerrainMesh::ApplyMaskedSimplex(mesh,
 *		0.008f,		// Controls the "detail" of the terrain
 *		15.0f,		// Controls the max elevation
 *		3000,		// For the terrain shape
 *		3000,		// For the island mask
 *		0.001f,		// Controls the "spread" of the mask — lower = fewer islands
 *		1.5f		// Controls how aggressive the mask is — HIGHER = MORE water
 *	);
*
*/
namespace wf
{
	struct Mesh;

	namespace mesh
	{
		class Terrain
		{
		public:
			// the main mesh, first.
			static std::shared_ptr<Mesh> createPlane(int width, int height, int resolution, float tileFactor = 20.f);

			// centers the mesh (using its size) at 0,0
			static void applyCenter(std::shared_ptr<Mesh> mesh, const Vec2& size);

			// if there's anything below a certain y value, the whole thing gets pushed up until it isn't.
			static void applyMinY(std::shared_ptr<Mesh> mesh, float y);

			// adjust Y by specific amount up/down
			static void applyAdjustY(std::shared_ptr<Mesh> mesh, float yDiff);

			// noise! fine-grained basics, or more involved versions that utilitise them.
			static void applyPerlinNoise(std::shared_ptr<Mesh> mesh, float frequency, float amplitude, int seed);
			static void applySimplexNoise(std::shared_ptr<Mesh> mesh, float frequency, float amplitude, int seed);
			static void applyFractalSimplexNoise(std::shared_ptr<Mesh> mesh, float baseFreq, float amplitude, int seed, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f);
			static void applyMaskedSimplex(std::shared_ptr<Mesh> mesh, float baseFreq, float amplitude, int seed, int maskSeed, float maskFreq, float maskStrength = 1.0f);

			// applies "edging" around the plain to hide the fact that it's just a big square, really.
			static void applyEdging(std::shared_ptr<Mesh> mesh, Vec2 size, float degree = 10.f);

			// wrapping-up function; essentially fixes all of the key stuff that might have been affected by brutally yanking everything around
			static void fixTerrainNormalsAndUVsAndStuff(std::shared_ptr<Mesh>, const Vec2& flatDimensions, float tileFactor = 20.f);

		private:
			Terrain() = delete;

			// helpers for the above
			static float fractalNoise2D(float (*noiseFunc)(float, float), float x, float y, int octaves, float persistence, float lacunarity);
		};
	}
}