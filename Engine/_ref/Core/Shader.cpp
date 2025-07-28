#include "pch.h"
#include "Core.h"
#include "Shader.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace wf
{
	static std::unordered_map<std::string, Shader> g_shaderRegistry;

	Shader loadShader(const char* vertexShader, const char* fragmentShader)
	{
		std::string vsSourceStr, fsSourceStr;

		std::ifstream vsFile(vertexShader);
		if (!vsFile) throw std::runtime_error("Failed to open vertex shader");
		vsSourceStr = std::string((std::istreambuf_iterator<char>(vsFile)),
			std::istreambuf_iterator<char>());
		vsFile.close();

		std::ifstream fsFile(fragmentShader);
		if (!fsFile) throw std::runtime_error("Failed to open fragment shader");
		fsSourceStr = std::string((std::istreambuf_iterator<char>(fsFile)),
			std::istreambuf_iterator<char>());
		fsFile.close();

		return loadShaderFromString(vsSourceStr.c_str(), fsSourceStr.c_str());
	}

	Shader loadShaderFromString(const char* vertexShader, const char* fragmentShader)
	{
		Shader shader;
		shader.handle = glCreateProgram();

		if (vertexShader) {
			auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
			if (vs) {
				glAttachShader(shader.handle, vs);
				glDeleteShader(vs);
			}
		}
		if (fragmentShader) {
			auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
			if (fs) {
				glAttachShader(shader.handle, fs);
				glDeleteShader(fs);
			}
		}

		glLinkProgram(shader.handle);

		int success;
		glGetProgramiv(shader.handle, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(shader.handle, 512, NULL, infoLog);
			throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
		}
		return shader;
	}

	void destroyShader(Shader& shader)
	{
		if (shader.handle) {
			glDeleteProgram(shader.handle);
			shader.handle = {};
		}
	}

	void useShader(const Shader& shader)
	{
		if (!shader.handle) throw std::runtime_error("Unmanaged shader cannot be used");
		glUseProgram(shader.handle);
	}

	void registerShader(const std::string& name, Shader shader)
	{
		g_shaderRegistry[name] = shader;
	}

	Shader* getShader(const std::string& name)
	{
		auto it = g_shaderRegistry.find(name);
		if (it == g_shaderRegistry.end()) return nullptr;
		return &it->second;
	}

	void unregisterShader(const std::string& name)
	{
		auto shader = getShader(name);
		if (shader) {
			destroyShader(*shader);
			g_shaderRegistry.erase(name);
		}
	}

	void destroyShaderRegistry()
	{
		std::vector<std::string> keys;
		keys.reserve(g_shaderRegistry.size());
		for (auto& [k, s] : g_shaderRegistry) keys.push_back(k);
		for (auto& k : keys) unregisterShader(k);
	}

	int getShaderUniformLocation(const Shader& shader, const char* name)
	{
		return glGetUniformLocation(shader.handle, name);
	}

	int getShaderAttribLocation(const Shader& shader, const char* name)
	{
		return glGetAttribLocation(shader.handle, name);
	}

	void setShaderUniform(const Shader& shader, int loc, const Vec2& value)
	{
		glUseProgram(shader.handle);
		glUniform2f(
			loc,
			value.x, value.y
		);
	}

	void setShaderUniform(const Shader& shader, int loc, const Vec3& value)
	{
		glUseProgram(shader.handle);
		glUniform3f(
			loc,
			value.x, value.y, value.z
		);
	}

	void setShaderUniform(const Shader& shader, int loc, const Vec4& value)
	{
		glUseProgram(shader.handle);
		glUniform4f(
			loc,
			value.x, value.y, value.z, value.w
		);
	}

	void setShaderUniform(const Shader& shader, int loc, const Colour& value)
	{
		glUseProgram(shader.handle);
		glUniform4f(
			loc,
			value.r, value.g, value.b, value.a
		);
	}

	void setShaderUniform(const Shader& shader, int loc, const Mat4& value)
	{
		glUseProgram(shader.handle);
		glUniformMatrix4fv(
			loc,
			1,
			GL_FALSE,
			&value.matrix[0][0]
		);
	}

	void setShaderUniform(const Shader& shader, int loc, int value)
	{
		glUseProgram(shader.handle);
		glUniform1i(
			loc,
			value
		);
	}

	void setShaderUniform(const Shader& shader, int loc, bool value)
	{
		setShaderUniform(shader, loc, (int)value);
	}

	void setShaderUniform(const Shader& shader, int loc, float value)
	{
		glUseProgram(shader.handle);
		glUniform1f(
			loc,
			value
		);
	}

	GLint compileShader(GLenum type, const char* source)
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
}