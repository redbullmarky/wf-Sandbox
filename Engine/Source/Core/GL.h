#pragma once
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"

#include <glm/glm.hpp>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#define GL_CHECK()																\
{																				\
    GLenum err;																	\
    while ((err = glGetError()) != GL_NO_ERROR) {								\
        fprintf(stderr, "GL ERROR: 0x%X at %s:%d\n", err, __FILE__, __LINE__);	\
    }																			\
}

namespace wf::wgl
{
	extern std::stack<glm::ivec4> g_viewportStack;

	struct MeshBufferHandle
	{
		unsigned int vao{};
		unsigned int vbo{};
		unsigned int ebo{};
	};

	enum class TextureWrap
	{
		DEFAULT = -1,
		REPEAT = 0,
		CLAMP,
		MIRROR_REPEAT,
		MIRROR_CLAMP
	};

	enum class TextureFormat
	{
		AUTO = -1,	// let's the loader do its thing instead of being specific
		RGBA8 = 0,
		RGB8,
		RGBA16F,
		RGBA32F,
		DEPTH24,
		DEPTH32F
	};

	enum class TextureFilter
	{
		LINEAR = 0,
		NEAREST,
		ANISOTROPIC_4X,
		ANISOTROPIC_8X,
		ANISOTROPIC_16X
	};

	struct TextureHandle
	{
		unsigned int glId{};
		int width{};
		int height{};

		int format{};
		int internalFormat{};
	};

	enum class BlendMode
	{
		OPAQUE = 0,		///< No blending, the source color fully replaces the destination color.
		ALPHA,			///< Alpha blending: source color is blended with the destination based on alpha value.
		ADDITIVE,		///< Additive blending: source color is added to the destination, making bright effects.
		MULTIPLY		///< Multiply blending: source color is multiplied with the destination, darkening the image.
	};

	enum class CullMode
	{
		NONE,
		BACK,
		FRONT
	};

	enum class DepthFunc
	{
		LESS = 0,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS,
		NEVER
	};

	struct ShaderHandle
	{
		unsigned int glId{};
	};

	struct RenderTargetHandle
	{
		TextureHandle colourTexture{};
		TextureHandle depthTexture{};

		bool colour{ false };
		bool depth{ false };

		int width{};
		int height{};

		unsigned int fbo{};
	};

	bool init();

	void setViewport(int w, int h);
	void setViewport(int x, int y, int w, int h);
	void pushViewport();
	void popViewport();

	void enableDepthTest(bool enable = true);
	void enableDepthMask(bool enable = true);
	void setDepthFunc(DepthFunc func);
	void setCullMode(CullMode mode);
	void setBlendMode(BlendMode mode);

	void clear(bool colour = true, bool depth = true);
	void clearColour(const Colour& colour, bool clearDepth = false);

	[[nodiscard]] MeshBufferHandle createMeshBuffers();
	void destroyMeshBuffers(MeshBufferHandle& buffers);
	void uploadMeshData(
		const MeshBufferHandle& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		bool dynamic = false
	);
	void updateMeshData(
		const MeshBufferHandle& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices
	);
	void drawMeshBuffers(const MeshBufferHandle& buffers, unsigned int vertexCount, unsigned int indexCount, bool wireframe = false);

	[[nodiscard]] ShaderHandle loadShader(const char* vertFilename, const char* fragFilename);
	[[nodiscard]] ShaderHandle loadShaderFromString(const char* vertexShader, const char* fragmentShader);
	void useShader(const ShaderHandle& shader);
	void destroyShader(ShaderHandle& shader);
	int getShaderUniformLocation(const ShaderHandle& shader, const char* name);
	int getShaderAttribLocation(const ShaderHandle& shader, const char* name);
	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec2& value);
	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec3& value);
	void setShaderUniform(const ShaderHandle& shader, int loc, const Vec4& value);
	void setShaderUniform(const ShaderHandle& shader, int loc, const Colour& value);
	void setShaderUniform(const ShaderHandle& shader, int loc, const Mat4& value);
	void setShaderUniform(const ShaderHandle& shader, int loc, int value);
	void setShaderUniform(const ShaderHandle& shader, int loc, bool value);
	void setShaderUniform(const ShaderHandle& shader, int loc, float value);

	[[nodiscard]] TextureHandle loadTextureData(
		const void* data,
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	[[nodiscard]] TextureHandle loadTexture(
		const char* filename,
		bool flipY = true,
		bool flipNormalMapY = false,
		TextureFormat format = TextureFormat::AUTO
	);

	[[nodiscard]] TextureHandle createColourTexture(
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	[[nodiscard]] TextureHandle createDepthTexture(
		unsigned int width,
		unsigned int height,
		TextureFormat format = TextureFormat::AUTO
	);

	void bindTexture(const TextureHandle& texture, int slot);
	void destroyTexture(TextureHandle& texture);

	[[nodiscard]] RenderTargetHandle createRenderTarget(
		int width,
		int height,
		bool colour = true,
		bool depth = false,
		TextureFormat colourFormat = TextureFormat::RGBA8,
		TextureFormat depthFormat = TextureFormat::DEPTH24
	);
	void blitRenderTarget(const RenderTargetHandle& target, int x, int y, int w, int h);
	void bindRenderTarget(const RenderTargetHandle& target);
	void unbindRenderTarget(const RenderTargetHandle& target);
	void destroyRenderTarget(RenderTargetHandle& target);
}
