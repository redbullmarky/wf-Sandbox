#pragma once
#include "GL/glew.h"

namespace wf
{
	enum class TextureWrap : int
	{
		DEFAULT = -1,
		REPEAT = 0,
		CLAMP,
		MIRROR_REPEAT,
		MIRROR_CLAMP
	};

	enum class TextureFormat :int
	{
		AUTO = -1,	// let's the loader do its thing instead of being specific
		RGBA8 = 0,
		RGB8,
		RGBA16F,
		RGBA32F,
		DEPTH24,
		DEPTH32F
	};

	enum TextureFilter :int
	{
		LINEAR = 0,
		NEAREST,
		ANISOTROPIC_4X,
		ANISOTROPIC_8X,
		ANISOTROPIC_16X
	};

	struct Texture
	{
		GLuint handle{};

		unsigned int width{ 0 };
		unsigned int height{ 0 };

		TextureFormat format{ TextureFormat::AUTO }; // informational; set to what gets set at load time
		TextureWrap wrap{ TextureWrap::DEFAULT };
		TextureFilter filter{ TextureFilter::LINEAR };

		bool hasMipmaps{ false }; // @todo not yet supported
	};

	Texture loadTextureData(
		const void* data,
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	Texture loadTexture(
		const char* filename,
		bool flipY = true,
		bool flipNormalMapY = false,
		TextureFormat format = TextureFormat::AUTO
	);

	Texture createColourTexture(
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	Texture createDepthTexture(
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	void bindTexture(const Texture& texture, int slot);
	void unloadTexture(Texture& texture);
}