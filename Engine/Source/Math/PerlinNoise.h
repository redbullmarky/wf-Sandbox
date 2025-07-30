#pragma once
constexpr auto PERLIN_SIZE = 256;

namespace wf
{
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
}