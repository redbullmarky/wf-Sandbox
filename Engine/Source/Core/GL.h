#pragma once
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"
#include "Render/Texture.h"

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
	struct MeshBuffers
	{
		unsigned int vao{};
		unsigned int vbo{};
		unsigned int ebo{};
	};

	bool init();

	void setViewport(int w, int h);
	void setViewport(int x, int y, int w, int h);

	void enableDepthTest(bool enable = true);

	void clear(bool colour = true, bool depth = true);
	void clearColour(const Colour& colour, bool clearDepth = false);

	[[nodiscard]] MeshBuffers createMeshBuffers();
	void destroyMeshBuffers(MeshBuffers& buffers);
	void uploadMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		bool dynamic = false
	);
	void updateMeshData(
		const MeshBuffers& buffers,
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices
	);
	void drawMeshBuffers(const MeshBuffers& buffers, unsigned int vertexCount, unsigned int indexCount, bool wireframe = false);

	[[nodiscard]] unsigned int loadShader(const char* vertFilename, const char* fragFilename);
	[[nodiscard]] unsigned int loadShaderFromString(const char* vertexShader, const char* fragmentShader);
	void useShader(unsigned int shader);
	void destroyShader(unsigned int shader);
	int getShaderUniformLocation(unsigned int shader, const char* name);
	int getShaderAttribLocation(unsigned int shader, const char* name);
	void setShaderUniform(unsigned int shader, int loc, const Vec2& value);
	void setShaderUniform(unsigned int shader, int loc, const Vec3& value);
	void setShaderUniform(unsigned int shader, int loc, const Vec4& value);
	void setShaderUniform(unsigned int shader, int loc, const Colour& value);
	void setShaderUniform(unsigned int shader, int loc, const Mat4& value);
	void setShaderUniform(unsigned int shader, int loc, int value);
	void setShaderUniform(unsigned int shader, int loc, bool value);
	void setShaderUniform(unsigned int shader, int loc, float value);

	void bindTexture(const Texture& texture, int slot);
	void destroyTexture(Texture& texture);
}