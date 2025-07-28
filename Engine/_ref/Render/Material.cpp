#include "pch.h"
#include "Material.h"

namespace wf
{
	void PhongMaterial::use()
	{
		useShader(*shader);

		// DIFFUSE
		if (shader->isValidLocation("hasDiffuseMap") && shader->isValidLocation("diffuseMap")) {
			setShaderUniform(*shader, shader->locs["hasDiffuseMap"], diffuse.texture ? true : false);

			if (diffuse.texture) {
				bindTexture(*diffuse.texture, 0);
				setShaderUniform(
					*shader,
					shader->locs["diffuseMap"],
					0
				);
			}
		}

		if (shader->isValidLocation("diffuseColour")) {
			setShaderUniform(
				*shader,
				shader->locs["diffuseColour"],
				diffuse.colour
			);
		}

		// NORMAL
		if (shader->isValidLocation("hasNormalMap") && shader->isValidLocation("normalMap")) {
			setShaderUniform(*shader, shader->locs["hasNormalMap"], normal.texture ? true : false);

			if (normal.texture) {
				bindTexture(*normal.texture, 1);
				setShaderUniform(
					*shader,
					shader->locs["normalMap"],
					1
				);
			}

			if (shader->isValidLocation("normalStrength")) {
				setShaderUniform(
					*shader,
					shader->locs["normalStrength"],
					normal.strength
				);
			}
		}

		// SPECULAR
		if (shader->isValidLocation("hasSpecularMap") && shader->isValidLocation("specularMap")) {
			setShaderUniform(*shader, shader->locs["hasSpecularMap"], specular.texture ? true : false);

			if (specular.texture) {
				bindTexture(*specular.texture, 2);
				setShaderUniform(
					*shader,
					shader->locs["specularMap"],
					2
				);
			}
		}

		if (shader->isValidLocation("specularColour")) {
			setShaderUniform(
				*shader,
				shader->locs["specularColour"],
				specular.colour
			);
		}
		if (shader->isValidLocation("specularShininess")) {
			setShaderUniform(
				*shader,
				shader->locs["specularShininess"],
				specular.shininess
			);
		}
		if (shader->isValidLocation("specularIntensity")) {
			setShaderUniform(
				*shader,
				shader->locs["specularIntensity"],
				specular.intensity
			);
		}
	}

	void OutlineMaterial::use()
	{
		useShader(*shader);

		if (shader->isValidLocation("outlineThickness") && shader->isValidLocation("outlineColour")) {
			setShaderUniform(
				*shader,
				shader->locs["outlineThickness"],
				outlineThickness
			);
			setShaderUniform(
				*shader,
				shader->locs["outlineColour"],
				outlineColour
			);
		}
	}
}
