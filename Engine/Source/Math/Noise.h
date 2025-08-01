#pragma once

constexpr auto PERLIN_SIZE = 256;

namespace wf
{
	/**
	 * @brief Simple Perline noise implementation
	 */
	class PerlinNoise
	{
	public:
		PerlinNoise(int seed = 0);
		~PerlinNoise() = default;

		static void setSeed(int seed);
		static float getValue(float x, float y);

	private:
		static float fade(float t);
		static float lerp(float a, float b, float t);
		static float grad(int hash, float x, float y);

	private:
		inline static int m_Seed{ 0 };
		inline static int p[PERLIN_SIZE * 2];
	};

	/**
	 * @brief Simple Simplex noise implementation
	 */
	class SimplexNoise
	{
	public:
		SimplexNoise(int seed);
		~SimplexNoise() = default;

		static void setSeed(int inseed);
		static float getValue(float x, float y);

	private:
		static float dot(const int* g, float x, float y);

	public:
		inline static int seed{ 0 };

	private:
		inline static int perm[512];

		// Gradients for 2D. They approximate the directions to the
		// vertices of an octagon from the center.
		inline static const int grad3[12][2] = {
		   {1,1}, {-1,1}, {1,-1}, {-1,-1},
		   {1,0}, {-1,0}, {1,0}, {-1,0},
		   {0,1}, {0,-1}, {0,1}, {0,-1}
		};
	};
}