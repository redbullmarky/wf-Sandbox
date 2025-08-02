#include "pch.h"
#include "Render.h"

namespace wf
{
	Texture loadTexture(const char* filename)
	{
		Texture texture;
		texture.handle = wgl::loadTexture(filename);
		return texture;
	}
}