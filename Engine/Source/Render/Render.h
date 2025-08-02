#pragma once
#include "Texture.h"

#include <memory>

namespace wf
{
	[[nodiscard]] std::shared_ptr<Texture> loadTexture(const char* filename);
}