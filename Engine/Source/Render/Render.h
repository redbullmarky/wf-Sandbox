#pragma once
#include "Texture.h"

#include <memory>

namespace wf
{
	[[nodiscard]] Texture loadTexture(const char* filename);
}