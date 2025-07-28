#pragma once
#include "Math/Math.h"
#include "Render/Colour.h"

#include <GL/glew.h>
#include <string>
#include <unordered_map>

namespace wf
{
	struct Shader
	{
		GLint handle{ 0 };

		std::unordered_map<std::string, int> locs;

		bool isValidLocation(std::string loc)
		{
			return locs.contains(loc) && locs[loc] >= 0;
		}
	};

	extern std::unordered_map<std::string, Shader> g_shaderRegistry;

	Shader loadShader(const char* vertFilename, const char* fragFilename);
	Shader loadShaderFromString(const char* vertexShader, const char* fragmentShader);
	void useShader(const Shader& shader);
	void destroyShaderRegistry();

	void registerShader(const std::string& name, Shader shader);
	Shader* getShader(const std::string& name);
	void unregisterShader(const std::string& name);

	int getShaderUniformLocation(const Shader& shader, const char* name);
	int getShaderAttribLocation(const Shader& shader, const char* name);
	void setShaderUniform(const Shader& shader, int loc, const Vec2& value);
	void setShaderUniform(const Shader& shader, int loc, const Vec3& value);
	void setShaderUniform(const Shader& shader, int loc, const Vec4& value);
	void setShaderUniform(const Shader& shader, int loc, const Colour& value);
	void setShaderUniform(const Shader& shader, int loc, const Mat4& value);
	void setShaderUniform(const Shader& shader, int loc, int value);
	void setShaderUniform(const Shader& shader, int loc, bool value);
	void setShaderUniform(const Shader& shader, int loc, float value);

	void destroyShader(Shader& shader);

	GLint compileShader(GLenum type, const char* source);
}
