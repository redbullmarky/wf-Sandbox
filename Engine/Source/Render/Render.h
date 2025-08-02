#pragma once
#include "Shader.h"
#include "Texture.h"

#include <memory>


/**
 * @todo These things would typically reload the same things. If we were to at least cache the wgl resource, we'd be golden
 */
namespace wf
{
	[[nodiscard]] Texture loadTexture(const char* filename);

	[[nodiscard]] Shader loadShader(const char* vertFilename, const char* fragFilename);
	[[nodiscard]] Shader loadPhongShader();
	[[nodiscard]] Shader loadBasicShader();
}