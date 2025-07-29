#include "pch.h"
#include "GL.h"

#include "Geometry/Geometry.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace wf::wgl
{
	std::stack<glm::ivec4> g_viewportStack;

	bool init()
	{
		GLenum result = glewInit();
		return result == GLEW_OK;
	}

	void setViewport(int w, int h)
	{
		setViewport(0, 0, w, h);
	}

	void setViewport(int x, int y, int w, int h)
	{
		glViewport(x, y, w, h);
	}

	void pushViewport()
	{
		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		g_viewportStack.push(glm::ivec4(vp[0], vp[1], vp[2], vp[3]));
	}

	void popViewport()
	{
		if (g_viewportStack.empty())
			throw std::runtime_error("Viewport stack underflow");

		auto vp = g_viewportStack.top();
		g_viewportStack.pop();
		glViewport(vp.x, vp.y, vp.z, vp.w);
	}

	void enableDepthTest(bool enable)
	{
		if (enable) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	void enableDepthMask(bool enable)
	{
		glDepthMask(enable ? GL_TRUE : GL_FALSE);
	}

	void setCullMode(CullMode mode)
	{
		switch (mode)
		{
		case CullMode::NONE:
			glDisable(GL_CULL_FACE);
			break;
		case CullMode::BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CullMode::FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		}
	}

	void setBlendMode(BlendMode mode)
	{
		switch (mode)
		{
		case BlendMode::OPAQUE:
			glDisable(GL_BLEND);
			break;
		case BlendMode::ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::ADDITIVE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case BlendMode::MULTIPLY:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		}
	}

	void clear(bool colour, bool depth)
	{
		GLbitfield flags{};
		if (colour) flags |= GL_COLOR_BUFFER_BIT;
		if (depth) flags |= GL_DEPTH_BUFFER_BIT;

		glClear(flags);
	}

	void clearColour(const Colour& colour, bool clearDepth)
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);

		clear(true, clearDepth);
	}

	MeshBufferHandle createMeshBuffers()
	{
		MeshBufferHandle buffers;

		glGenVertexArrays(1, &buffers.vao);
		glGenBuffers(1, &buffers.vbo);
		glGenBuffers(1, &buffers.ebo);

		glBindVertexArray(buffers.vao);
		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glBindVertexArray(0);

		return buffers;
	}

	void destroyMeshBuffers(MeshBufferHandle& buffers)
	{
		if (buffers.vao) glDeleteVertexArrays(1, &buffers.vao);
		if (buffers.vbo) glDeleteBuffers(1, &buffers.vbo);
		if (buffers.ebo) glDeleteBuffers(1, &buffers.ebo);
		buffers = {};
	}

	void uploadMeshData(
		const MeshBufferHandle& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		bool dynamic
	) {
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot upload to non-initialised buffers");
		if (!buffers.ebo && indices.size()) throw std::runtime_error("Indices provided but no ebo created");

		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if (indices.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
	}

	void updateMeshData(
		const MeshBufferHandle& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices
	) {
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot update non-initialised buffers");
		if (!buffers.ebo && indices.size()) throw std::runtime_error("Indices provided but no ebo created");

		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		if (indices.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());
		}
	}

	void drawMeshBuffers(const MeshBufferHandle& buffers, unsigned int vertexCount, unsigned int indexCount, bool wireframe)
	{
		if (!buffers.vao || !buffers.vbo) throw std::runtime_error("Cannot draw non-initialised buffers");
		if (!buffers.ebo && indexCount) throw std::runtime_error("Indices provided but no ebo created");

		glBindVertexArray(buffers.vao);

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (buffers.ebo)
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(0);
	}

	[[nodiscard]] static GLuint compileShader(GLenum type, const char* source)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
		}
		return shader;
	}

	ShaderHandle loadShader(const char* vertFilename, const char* fragFilename)
	{
		std::string vsSourceStr, fsSourceStr;

		std::ifstream vsFile(vertFilename);
		if (!vsFile) throw std::runtime_error("Failed to open vertex shader");
		vsSourceStr = std::string((std::istreambuf_iterator<char>(vsFile)),
			std::istreambuf_iterator<char>());
		vsFile.close();

		std::ifstream fsFile(fragFilename);
		if (!fsFile) throw std::runtime_error("Failed to open fragment shader");
		fsSourceStr = std::string((std::istreambuf_iterator<char>(fsFile)),
			std::istreambuf_iterator<char>());
		fsFile.close();

		return loadShaderFromString(vsSourceStr.c_str(), fsSourceStr.c_str());
	}

	ShaderHandle loadShaderFromString(const char* vertexShader, const char* fragmentShader)
	{
		ShaderHandle handle{ glCreateProgram() };

		if (vertexShader) {
			auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
			if (vs) {
				glAttachShader(handle.glId, vs);
				glDeleteShader(vs);
			}
		}
		if (fragmentShader) {
			auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
			if (fs) {
				glAttachShader(handle.glId, fs);
				glDeleteShader(fs);
			}
		}

		glLinkProgram(handle.glId);

		int success;
		glGetProgramiv(handle.glId, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(handle.glId, 512, NULL, infoLog);
			throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
		}

		glValidateProgram(handle.glId);

		GLint validated = 0;
		glGetProgramiv(handle.glId, GL_VALIDATE_STATUS, &validated);
		if (!validated) {
			char log[512];
			glGetProgramInfoLog(handle.glId, sizeof(log), nullptr, log);
			throw std::runtime_error("Shader validation failed: " + std::string(log));
		}

		return handle;
	}

	void useShader(const ShaderHandle& shader)
	{
		if (shader.glId <= 0) throw std::runtime_error("Unmanaged shader cannot be used");
		glUseProgram(shader.glId);
	}

	void destroyShader(ShaderHandle& shader)
	{
		if (shader.glId >= 0) {
			glDeleteProgram(shader.glId);
			shader.glId = {};
		}
	}

	int getShaderUniformLocation(const ShaderHandle& shader, const char* name)
	{
		return glGetUniformLocation(shader.glId, name);
	}

	int getShaderAttribLocation(const ShaderHandle& shader, const char* name)
	{
		return glGetAttribLocation(shader.glId, name);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec2& value)
	{
		glUseProgram(shader.glId);
		glUniform2f(
			loc,
			value.x, value.y
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec3& value)
	{
		glUseProgram(shader.glId);
		glUniform3f(
			loc,
			value.x, value.y, value.z
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec4& value)
	{
		glUseProgram(shader.glId);
		glUniform4f(
			loc,
			value.x, value.y, value.z, value.w
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, const Colour& value)
	{
		glUseProgram(shader.glId);
		glUniform4f(
			loc,
			value.r, value.g, value.b, value.a
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, const Mat4& value)
	{
		glUseProgram(shader.glId);
		glUniformMatrix4fv(
			loc,
			1,
			GL_FALSE,
			&value.matrix[0][0]
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, int value)
	{
		glUseProgram(shader.glId);
		glUniform1i(
			loc,
			value
		);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, bool value)
	{
		setShaderUniform(shader, loc, (int)value);
	}

	void setShaderUniform(const ShaderHandle& shader, int loc, float value)
	{
		glUseProgram(shader.glId);
		glUniform1f(
			loc,
			value
		);
	}

	TextureHandle loadTextureData(const void* data, unsigned int width, unsigned int height, TextureFormat format)
	{
		TextureHandle texture;

		glGenTextures(1, &texture.glId);
		glBindTexture(GL_TEXTURE_2D, texture.glId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		texture.width = width;
		texture.height = height;

		texture.format = format == TextureFormat::RGBA8 ? GL_RGBA : GL_RGB;
		texture.internalFormat = format == TextureFormat::RGBA8 ? GL_RGBA8 : GL_RGB8;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			texture.internalFormat,
			width,
			height,
			0,
			texture.format,
			GL_UNSIGNED_BYTE,
			data
		);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	TextureHandle loadTexture(const char* filename, bool flipY, bool flipNormalMapY, TextureFormat format)
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

		TextureHandle texture = loadTextureData(surface->pixels, surface->w, surface->h, finalFormat);

		SDL_DestroySurface(surface);
		return texture;
	}

	TextureHandle createColourTexture(unsigned int width, unsigned int height, TextureFormat format)
	{
		TextureHandle texture;

		texture.width = width;
		texture.height = height;

		glGenTextures(1, &texture.glId);
		glBindTexture(GL_TEXTURE_2D, texture.glId);

		texture.format = format == TextureFormat::RGBA8 ? GL_RGBA : GL_RGB;
		texture.internalFormat = format == TextureFormat::RGBA8 ? GL_RGBA8 : GL_RGB8;

		glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, width, height, 0, texture.format, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	TextureHandle createDepthTexture(unsigned int width, unsigned int height, TextureFormat format)
	{
		TextureHandle texture;

		texture.width = width;
		texture.height = height;
		texture.format = GL_DEPTH_COMPONENT;
		texture.internalFormat = GL_DEPTH_COMPONENT;

		glGenTextures(1, &texture.glId);
		glBindTexture(GL_TEXTURE_2D, texture.glId);

		GLenum glType = GL_FLOAT;

		if (format == TextureFormat::DEPTH24) {
			texture.internalFormat = GL_DEPTH_COMPONENT24;
			glType = GL_UNSIGNED_INT;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, texture.internalFormat, width, height, 0, texture.format, glType, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

	void bindTexture(const TextureHandle& texture, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture.glId);
	}

	void destroyTexture(TextureHandle& texture)
	{
		glDeleteTextures(1, &texture.glId);
		texture.glId = {};
	}

	RenderTargetHandle createRenderTarget(int width, int height, bool colour, bool depth, TextureFormat colourFormat, TextureFormat depthFormat)
	{
		RenderTargetHandle renderTarget;
		renderTarget.width = width;
		renderTarget.height = height;

		glGenFramebuffers(1, &renderTarget.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.fbo);

		if (colour) {
			renderTarget.colour = true;
			renderTarget.colourTexture = createColourTexture(width, height, colourFormat);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget.colourTexture.glId, 0);
		}
		else {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		if (depth) {
			renderTarget.depth = true;
			renderTarget.depthTexture = createDepthTexture(width, height, depthFormat);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderTarget.depthTexture.glId, 0);
		}


		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return renderTarget;
	}

	void blitRenderTarget(const RenderTargetHandle& target, int x, int y, int w, int h)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, target.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, target.width, target.height, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	void bindRenderTarget(const RenderTargetHandle& target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
		pushViewport();
		setViewport(target.width, target.height);

		GLint currentFBO = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
		if ((GLuint)currentFBO != target.fbo)
			throw std::runtime_error("FBO bind failed. Drawing to default framebuffer.");

	}

	void unbindRenderTarget(const RenderTargetHandle& target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		popViewport();
	}

	void destroyRenderTarget(RenderTargetHandle& target)
	{
		if (target.colourTexture.glId)
			destroyTexture(target.colourTexture);

		if (target.depthTexture.glId)
			destroyTexture(target.depthTexture);

		if (target.fbo) {
			glDeleteFramebuffers(1, &target.fbo);
			target.fbo = 0;
		}
	}
}
