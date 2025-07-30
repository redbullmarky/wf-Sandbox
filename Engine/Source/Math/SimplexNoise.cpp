#include "pch.h"
#include "SimplexNoise.h"

#include <cmath>
#include <stdlib.h>
#include <utility>

namespace wf
{
	SimplexNoise::SimplexNoise(int seed)
	{
		setSeed(seed);
	}

	void SimplexNoise::setSeed(int inseed)
	{
		seed = inseed;
		srand(seed);
		for (int i = 0; i < 256; i++) perm[i] = i;

		for (int i = 0; i < 256; i++) {
			int j = rand() % 256;
			std::swap(perm[i], perm[j]);
		}

		for (int i = 0; i < 256; i++) perm[256 + i] = perm[i];
	}

	float SimplexNoise::dot(const int* g, float x, float y)
	{
		return g[0] * x + g[1] * y;
	}

	float SimplexNoise::getValue(float xin, float yin)
	{
		const float F2 = 0.5f * (sqrtf(3.0f) - 1.0f);
		const float G2 = (3.0f - sqrtf(3.0f)) / 6.0f;

		float s = (xin + yin) * F2;
		int i = (int)floor(xin + s);
		int j = (int)floor(yin + s);

		float t = (i + j) * G2;
		float X0 = i - t;
		float Y0 = j - t;
		float x0 = xin - X0;
		float y0 = yin - Y0;

		int i1, j1;
		if (x0 > y0) { i1 = 1; j1 = 0; }
		else { i1 = 0; j1 = 1; }

		float x1 = x0 - i1 + G2;
		float y1 = y0 - j1 + G2;
		float x2 = x0 - 1.0f + 2.0f * G2;
		float y2 = y0 - 1.0f + 2.0f * G2;

		int ii = i & 255;
		int jj = j & 255;
		int gi0 = perm[ii + perm[jj]] % 12;
		int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
		int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

		float t0 = 0.5f - x0 * x0 - y0 * y0;
		float n0 = (t0 < 0) ? 0.0f : powf(t0, 4.0f) * dot(grad3[gi0], x0, y0);

		float t1 = 0.5f - x1 * x1 - y1 * y1;
		float n1 = (t1 < 0) ? 0.0f : powf(t1, 4.0f) * dot(grad3[gi1], x1, y1);

		float t2 = 0.5f - x2 * x2 - y2 * y2;
		float n2 = (t2 < 0) ? 0.0f : powf(t2, 4.0f) * dot(grad3[gi2], x2, y2);

		// Scale result to [-1,1]
		return 70.0f * (n0 + n1 + n2);
	}
}