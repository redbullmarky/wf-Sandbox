#include "pch.h"
#include "Texture.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace wf
{
	Texture loadTextureData(const void* data, unsigned int width, unsigned int height, TextureFormat format)
	{
		Texture texture;

		glGenTextures(1, &texture.handle);
		glBindTexture(GL_TEXTURE_2D, texture.handle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		texture.width = width;
		texture.height = height;

		GLint glFormat = format == TextureFormat::RGBA8 ? GL_RGBA : GL_RGB;
		GLint intGlFormat = format == TextureFormat::RGBA8 ? GL_RGBA8 : GL_RGB8;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			intGlFormat,
			width,
			height,
			0,
			glFormat,
			GL_UNSIGNED_BYTE,
			data
		);

		texture.format = format;

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	Texture loadTexture(const char* filename, bool flipY, bool flipNormalMapY, TextureFormat format)
	{
		SDL_Surface* surface = IMG_Load(filename);
		if (!surface) {
			throw std::runtime_error("Cannot load texture " + std::string(SDL_GetError()));
		}

		uint8_t* pixels = static_cast<uint8_t*>(surface->pixels);
		int pitch = surface->pitch;

		// flip Y...
		if (flipY) {
			std::vector<uint8_t> temp(pitch);

			for (int y = 0; y < surface->h / 2; y++) {
				uint8_t* rowTop = pixels + y * pitch;
				uint8_t* rowBottom = pixels + (surface->h - y - 1) * pitch;

				memcpy(temp.data(), rowTop, pitch);
				memcpy(rowTop, rowBottom, pitch);
				memcpy(rowBottom, temp.data(), pitch);
			}
		}
		if (flipNormalMapY) {
			for (int y = 0; y < surface->h; y++) {
				uint8_t* pixels = static_cast<uint8_t*>(surface->pixels);
				uint8_t* row = pixels + y * pitch;
				for (int x = 0; x < surface->w; x++) {
					uint8_t* texel = row + x * SDL_BYTESPERPIXEL(surface->format);
					texel[1] = 255 - texel[1];
				}
			}
		}

		GLint glFormat = SDL_BYTESPERPIXEL(surface->format) == 4 ? GL_RGBA : GL_RGB;

		// AUTO resolves based on loaded image if requested
		TextureFormat finalFormat = format == TextureFormat::AUTO ?
			(glFormat == GL_RGBA ? TextureFormat::RGBA8 : TextureFormat::RGB8) :
			format;

		Texture texture = loadTextureData(surface->pixels, surface->w, surface->h, finalFormat);

		SDL_DestroySurface(surface);
		return texture;
	}

	Texture createColourTexture(unsigned int width, unsigned int height, TextureFormat format)
	{
		Texture texture;

		texture.width = width;
		texture.height = height;
		texture.format = format;

		glGenTextures(1, &texture.handle);
		glBindTexture(GL_TEXTURE_2D, texture.handle);

		GLint glFormat = format == TextureFormat::RGBA8 ? GL_RGBA : GL_RGB;
		GLint intGlFormat = format == TextureFormat::RGBA8 ? GL_RGBA8 : GL_RGB8;

		glTexImage2D(GL_TEXTURE_2D, 0, intGlFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	Texture createDepthTexture(unsigned int width, unsigned int height, TextureFormat format)
	{
		Texture texture;

		texture.width = width;
		texture.height = height;
		texture.format = format;

		glGenTextures(1, &texture.handle);
		glBindTexture(GL_TEXTURE_2D, texture.handle);

		GLenum internalFormat = GL_DEPTH_COMPONENT;
		GLenum glType = GL_FLOAT;

		if (format == TextureFormat::DEPTH24)
		{
			internalFormat = GL_DEPTH_COMPONENT24;
			glType = GL_UNSIGNED_INT;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_DEPTH_COMPONENT, glType, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	void bindTexture(const Texture& texture, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture.handle);
	}

	void unloadTexture(Texture& texture)
	{
		glDeleteTextures(1, &texture.handle);
	}
}
