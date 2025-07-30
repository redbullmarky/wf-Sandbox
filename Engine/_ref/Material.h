#pragma once
#include "Colour.h"
#include "Core/Shader.h"
#include "Core/Texture.h"

#include <memory>

namespace wf
{
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
		GEQUAL,
		ALWAYS,
		NEVER
	};

	struct IMaterial
	{
		BlendMode blendMode{ BlendMode::OPAQUE };
		CullMode cullMode{ CullMode::BACK };
		bool depthMask{ true };
		bool depthTest{ true };
		DepthFunc depthFunc{ DepthFunc::LESS };

		bool wireframe{ false };

		Shader* shader;

		IMaterial(Shader* shader) : shader(shader) {}

		virtual void use() = 0;
	};

	struct PhongMaterial : public IMaterial
	{
		struct MaterialMapDiffuse
		{
			Texture* texture{ nullptr };
			Colour colour{ WHITE };
		} diffuse{};

		struct MaterialMapNormal
		{
			Texture* texture{ nullptr };
			float strength{ 1.f };
		} normal{};

		struct MaterialMapSpecular
		{
			Texture* texture{ nullptr };
			Colour colour{ WHITE };
			float shininess{ 32.f };
			float intensity{ 0.f };
		} specular{};

		virtual void use() override;

		static std::shared_ptr<PhongMaterial> create()
		{
			return std::shared_ptr<PhongMaterial>(new PhongMaterial());
		}

		PhongMaterial() : IMaterial(getShader("phong"))
		{
			shader->locs["mvp"] = getShaderUniformLocation(*shader, "mvp");
			shader->locs["matModel"] = getShaderUniformLocation(*shader, "matModel");

			// textures
			shader->locs["diffuseMap"] = getShaderUniformLocation(*shader, "diffuseMap");
			shader->locs["normalMap"] = getShaderUniformLocation(*shader, "normalMap");
			shader->locs["specularMap"] = getShaderUniformLocation(*shader, "specularMap");
			// flags
			shader->locs["hasDiffuseMap"] = getShaderUniformLocation(*shader, "hasDiffuseMap");
			shader->locs["hasNormalMap"] = getShaderUniformLocation(*shader, "hasNormalMap");
			shader->locs["hasSpecularMap"] = getShaderUniformLocation(*shader, "hasSpecularMap");
			// material props
			shader->locs["diffuseColour"] = getShaderUniformLocation(*shader, "diffuseColour");
			shader->locs["specularColour"] = getShaderUniformLocation(*shader, "specularColour");
			shader->locs["normalStrength"] = getShaderUniformLocation(*shader, "normalStrength");
			shader->locs["specularShininess"] = getShaderUniformLocation(*shader, "specularShininess");
			shader->locs["specularIntensity"] = getShaderUniformLocation(*shader, "specularIntensity");
			// lights & camera
			shader->locs["viewPos"] = getShaderUniformLocation(*shader, "viewPos");
			shader->locs["lightDir"] = getShaderUniformLocation(*shader, "lightDir");
			shader->locs["lightColour"] = getShaderUniformLocation(*shader, "lightColour");
			shader->locs["ambientLevel"] = getShaderUniformLocation(*shader, "ambientLevel");
		}
	};

	struct OutlineMaterial : public IMaterial
	{
		float outlineThickness{ 0.01f };
		Colour outlineColour{ BLACK };

		virtual void use() override;

		static std::shared_ptr<OutlineMaterial> create()
		{
			return std::shared_ptr<OutlineMaterial>(new OutlineMaterial());
		}

	private:
		OutlineMaterial() : IMaterial(getShader("outline"))
		{
			shader->locs["mvp"] = getShaderUniformLocation(*shader, "mvp");
			shader->locs["matModel"] = getShaderUniformLocation(*shader, "matModel");

			shader->locs["outlineThickness"] = getShaderUniformLocation(*shader, "outlineThickness");
			shader->locs["outlineColour"] = getShaderUniformLocation(*shader, "outlineColour");
		}
	};
}