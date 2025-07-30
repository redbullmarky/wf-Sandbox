#include "pch.h"
#include "PerlinNoise.h"

#include <cmath>
#include <stdlib.h>
#include <utility>

// @todo replace lerp with glm::mix or std::lerp
namespace wf
{
	PerlinNoise::PerlinNoise(int seed)
	{
		setSeed(seed);
	}

	void PerlinNoise::setSeed(int seed)
	{
		m_Seed = seed;

		srand(m_Seed);
		for (int i = 0; i < PERLIN_SIZE; i++) p[i] = i;

		for (int i = 0; i < PERLIN_SIZE; i++) {
			int j = rand() % PERLIN_SIZE;
			int tmp = p[i];
			p[i] = p[j];
			p[j] = tmp;
		}

		for (int i = 0; i < PERLIN_SIZE; i++) p[PERLIN_SIZE + i] = p[i];
	}

	float PerlinNoise::getValue(float x, float y)
	{
		int xi = (int)floor(x) & 255;
		int yi = (int)floor(y) & 255;

		float xf = x - floor(x);
		float yf = y - floor(y);

		float u = fade(xf);
		float v = fade(yf);

		int aa = p[p[xi] + yi];
		int ab = p[p[xi] + yi + 1];
		int ba = p[p[xi + 1] + yi];
		int bb = p[p[xi + 1] + yi + 1];

		float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
		float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

		return lerp(x1, x2, v);
	}

	float PerlinNoise::fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	float PerlinNoise::lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float PerlinNoise::grad(int hash, float x, float y)
	{
		int h = hash & 0x3F; // 6 bits
		float u = h < 4 ? x : y;
		float v = h < 4 ? y : x;
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}
}