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

	Material createBasicMaterial()
	{
		Material m;
		m.shader = loadBasicShader();
		return m;
	}

	Material createPhongMaterial()
	{
		Material m;
		m.shader = loadPhongShader();
		return m;
	}

	Shader loadPhongShader()
	{
		// @todo this'll eventually load from internals not files
		return loadPhongShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");
	}

	Shader loadPhongShader(const char* vertFilename, const char* fragFilename)
	{
		auto s = loadShader(vertFilename, fragFilename);

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

	Shader loadBasicShader()
	{
		// @todo this'll eventually load from internals not files
		return loadBasicShader("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	}

	Shader loadBasicShader(const char* vertFilename, const char* fragFilename)
	{
		auto s = loadShader(vertFilename, fragFilename);

		// transform
		s.locs["mvp"] = wgl::getShaderUniformLocation(s.handle, "mvp");
		s.locs["matModel"] = wgl::getShaderUniformLocation(s.handle, "matModel");

		// basic colour
		s.locs["diffuseColour"] = wgl::getShaderUniformLocation(s.handle, "diffuseColour");

		return s;
	}
}