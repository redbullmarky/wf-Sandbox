#pragma once
#include "Material.h"
#include "Scene/Component/CameraComponent.h"
#include "Scene/Component/LightComponent.h"
#include "Shader.h"
#include "Texture.h"

#include <memory>


/**
 * @todo These things would typically reload the same things. If we were to at least cache the wgl resource, we'd be golden
 */
namespace wf
{
	struct RenderContext
	{
		CameraComponent* camera;
		LightComponent* light;

		RenderContext(CameraComponent* camera, LightComponent* light) : camera(camera), light(light) {}
	};

	/**
	 * @brief Load a texture from an image
	 */
	[[nodiscard]] Texture loadTexture(const char* filename);

	/**
	 * @brief Load and compile a raw shader.
	 *
	 * Locations will need configuring manually to suit
	 */
	[[nodiscard]] Shader loadShader(const char* vertFilename, const char* fragFilename);

	/**
	 * @brief Material loaders
	 */
	[[nodiscard]] Material createBasicMaterial();
	[[nodiscard]] Material createPhongMaterial();

	/**
	 * @brief Load the standard internal phong shader or, if required, from custom files but with the locations configured.
	 *
	 * Using the filenames is  useful for dev, for now
	 */
	[[nodiscard]] Shader loadPhongShader();
	[[nodiscard]] Shader loadPhongShader(const char* vertFilename, const char* fragFilename);

	/**
	 * @brief Load the standard internal basic shader
	 * @return
	 */
	[[nodiscard]] Shader loadBasicShader();
	[[nodiscard]] Shader loadBasicShader(const char* vertFilename, const char* fragFilename);

}