#include "pch.h"
#include "MaterialTraits.h"
#include "Render/Render.h"

#include <cassert>

namespace wf
{
	bool DiffuseTrait::hasMap() const
	{
		return diffuse.map.handle.glId;
	}

	void DiffuseTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		if (shader.isValidLocation("diffuseColour")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("diffuseColour"),
				diffuse.colour
			);
		}
		else {
			throw std::runtime_error("nope");
		}

		bool hasDiffuseMap = hasMap();
		if (shader.isValidLocation("hasDiffuseMap")) {
			wgl::setShaderUniform(shader.handle, shader.location("hasDiffuseMap"), hasDiffuseMap);
		}
		if (!hasDiffuseMap || !shader.isValidLocation("diffuseMap")) return;

		wgl::bindTexture(diffuse.map.handle, 0);
		wgl::setShaderUniform(
			shader.handle,
			shader.location("diffuseMap"),
			0
		);
	}

	bool NormalTrait::hasMap() const
	{
		return normal.map.handle.glId;
	}

	void NormalTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		if (shader.isValidLocation("normalStrength")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("normalStrength"),
				normal.strength
			);
		}

		bool hasNormalMap = hasMap();
		if (shader.isValidLocation("hasNormalMap")) {
			wgl::setShaderUniform(shader.handle, shader.location("hasNormalMap"), hasNormalMap);
		}
		if (!hasNormalMap || !shader.isValidLocation("normalMap")) return;

		wgl::bindTexture(normal.map.handle, 1);
		wgl::setShaderUniform(
			shader.handle,
			shader.location("normalMap"),
			1
		);
	}

	bool SpecularTrait::hasMap() const
	{
		return specular.map.handle.glId;
	}

	void SpecularTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		if (shader.isValidLocation("specularColour")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("specularColour"),
				specular.colour
			);
		}

		if (shader.isValidLocation("specularShininess")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("specularShininess"),
				specular.shininess
			);
		}

		if (shader.isValidLocation("specularIntensity")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("specularIntensity"),
				specular.intensity
			);
		}

		bool hasSpecularMap = hasMap();
		if (shader.isValidLocation("hasSpecularMap")) {
			wgl::setShaderUniform(shader.handle, shader.location("hasSpecularMap"), hasSpecularMap);
		}
		if (!hasSpecularMap || !shader.isValidLocation("specularMap")) return;

		wgl::bindTexture(specular.map.handle, 2);
		wgl::setShaderUniform(
			shader.handle,
			shader.location("specularMap"),
			2
		);
	}

	void TransformTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		Mat4 matModel = transform.getTransformMatrix();
		Mat4 mvp(renderContext.camera->getViewProjectionMatrix() * matModel);

		if (shader.isValidLocation("matModel")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("matModel"),
				matModel
			);
		}

		if (shader.isValidLocation("mvp")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("mvp"),
				mvp
			);
		}
	}

	void LightingTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		if (shader.isValidLocation("lightVP")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("lightVP"),
				renderContext.light->getViewProjectionMatrix()
			);
		}

		if (shader.isValidLocation("lightDir")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("lightDir"),
				renderContext.light->getDirection()
			);
		}

		if (shader.isValidLocation("lightColour")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("lightColour"),
				renderContext.light->colour
			);
		}

		if (shader.isValidLocation("ambientLevel")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("ambientLevel"),
				renderContext.light->ambientLevel
			);
		}
	}

	void CameraTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		if (shader.isValidLocation("viewPos")) {
			wgl::setShaderUniform(
				shader.handle,
				shader.location("viewPos"),
				renderContext.camera->position
			);
		}
	}

	bool ShadowRecieverTrait::hasMap() const
	{
		return !shadow.shadowPass && shadow.map.fbo && shadow.map.depthTexture.glId;
	}

	void ShadowRecieverTrait::bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const
	{
		bool hasShadowMap = hasMap();
		if (shader.isValidLocation("hasShadowMap")) {
			wgl::setShaderUniform(shader.handle, shader.location("hasShadowMap"), hasShadowMap);
		}
		if (!hasShadowMap) return;

		if (shader.isValidLocation("shadowMap")) {
			wgl::bindTexture(shadow.map.depthTexture, 3);
			wgl::setShaderUniform(
				shader.handle,
				shader.location("shadowMap"),
				3
			);

			wgl::setShaderUniform(
				shader.handle,
				shader.location("shadowMapResolution"),
				(float)shadow.map.width
			);

			wgl::setShaderUniform(
				shader.handle,
				shader.location("shadowBias"),
				.005f
			);
		}
	}
}
