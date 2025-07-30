#include "Config.h"

namespace Squishies
{
	Config g_gameConfig;

	Config::Config()
	{
		gravity = -19.81f;

		worldBounds.extend({ -50.f, -10.f, -1.f });
		worldBounds.extend({ 50.f, 50.f, 1.f });

		spatialGridSize = 32.f;
	}

	Config& Config::get()
	{
		return g_gameConfig;
	}
}