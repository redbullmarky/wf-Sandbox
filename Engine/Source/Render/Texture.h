#pragma once
#include "Core/GL.h"

#include <memory>

namespace wf
{
	struct Texture
	{
		wgl::TextureHandle handle;							// internal ref

		int getWidth() const
		{
			return handle.width;
		}

		int getHeight() const
		{
			return handle.height;
		}

		static std::shared_ptr<Texture> create()
		{
			return std::shared_ptr<Texture>(new Texture());
		}

	private:
		Texture() = default;
	};
}