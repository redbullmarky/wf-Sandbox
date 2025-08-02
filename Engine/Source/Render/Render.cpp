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

	Shader loadShader(const char* vertFilename, const char* fragFilename)
	{
		Shader s;
		s.handle = wgl::loadShader(vertFilename, fragFilename);
		return s;
	}

	Shader wf::loadPhongShader()
	{
		auto s = loadShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");

		// transform
		s.locs["mvp"] = wgl::getShaderUniformLocation(s.handle, "mvp");
		s.locs["matModel"] = wgl::getShaderUniformLocation(s.handle, "matModel");

		// textures
		s.locs["diffuseMap"] = wgl::getShaderUniformLocation(s.handle, "diffuseMap");
		s.locs["normalMap"] = wgl::getShaderUniformLocation(s.handle, "normalMap");
		s.locs["specularMap"] = wgl::getShaderUniformLocation(s.handle, "specularMap");
		s.locs["shadowMap"] = wgl::getShaderUniformLocation(s.handle, "shadowMap");
		// flags
		s.locs["hasDiffuseMap"] = wgl::getShaderUniformLocation(s.handle, "hasDiffuseMap");
		s.locs["hasNormalMap"] = wgl::getShaderUniformLocation(s.handle, "hasNormalMap");
		s.locs["hasSpecularMap"] = wgl::getShaderUniformLocation(s.handle, "hasSpecularMap");
		s.locs["hasShadowMap"] = wgl::getShaderUniformLocation(s.handle, "hasShadowMap");
		// material props
		s.locs["diffuseColour"] = wgl::getShaderUniformLocation(s.handle, "diffuseColour");
		s.locs["specularColour"] = wgl::getShaderUniformLocation(s.handle, "specularColour");
		s.locs["normalStrength"] = wgl::getShaderUniformLocation(s.handle, "normalStrength");
		s.locs["specularShininess"] = wgl::getShaderUniformLocation(s.handle, "specularShininess");
		s.locs["specularIntensity"] = wgl::getShaderUniformLocation(s.handle, "specularIntensity");
		// lights & camera
		s.locs["viewPos"] = wgl::getShaderUniformLocation(s.handle, "viewPos");
		s.locs["lightDir"] = wgl::getShaderUniformLocation(s.handle, "lightDir");
		s.locs["lightColour"] = wgl::getShaderUniformLocation(s.handle, "lightColour");
		s.locs["ambientLevel"] = wgl::getShaderUniformLocation(s.handle, "ambientLevel");
		// shadows
		s.locs["lightVP"] = wgl::getShaderUniformLocation(s.handle, "lightVP");
		s.locs["shadowBias"] = wgl::getShaderUniformLocation(s.handle, "shadowBias");
		s.locs["shadowMapResolution"] = wgl::getShaderUniformLocation(s.handle, "shadowMapResolution");

		return s;
	}

	Shader wf::loadBasicShader()
	{
		auto s = loadShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");

		// transform
		s.locs["mvp"] = wgl::getShaderUniformLocation(s.handle, "mvp");
		s.locs["matModel"] = wgl::getShaderUniformLocation(s.handle, "matModel");

		return s;
	}
}