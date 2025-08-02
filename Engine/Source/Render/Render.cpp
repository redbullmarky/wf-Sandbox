#include "pch.h"
#include "Render.h"

namespace wf
{
	std::shared_ptr<Texture> loadTexture(const char* filename)
	{
		auto texture = Texture::create();
		texture->handle = wgl::loadTexture(filename);
		return texture;
	}
}