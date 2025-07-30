#pragma once
#include "Engine.h"

namespace Squishies
{
	struct Config
	{
		float gravity{};
		wf::BoundingBox worldBounds{};
		float spatialGridSize{};

		Config();
		static Config& get();
	};
	extern Config g_gameConfig;
}