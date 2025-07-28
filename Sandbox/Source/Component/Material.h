#pragma once
#include "Engine.h"

#include <memory>

namespace Sandbox::Component
{
	struct Material
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

		std::shared_ptr<wf::Shader> shader;

		struct Diffuse											// diffuse settings
		{
			std::shared_ptr<wf::Texture> map;						// diffuse texture
			wf::Colour colour{ wf::WHITE };							// base diffuse colour
		} diffuse;

		struct Normal											// normal map settings
		{
			std::shared_ptr<wf::Texture> map;						// normal map
			float strength{ 1.f };									// strength for applying map
		} normal;

		struct Specular											// specular settings
		{
			std::shared_ptr<wf::Texture> map;						// specular map
			wf::Colour colour{ wf::WHITE };							// specular colour
			float shininess{ 32.f };								// how shiny
			float intensity{ 0.f };									// level of specular. defaults to bland.
		} specular;

		BlendMode blendMode = BlendMode::OPAQUE;				// blend mode
		CullMode cullMode = CullMode::BACK;						// cull mode
		bool depthMask{ true };									// depth mask
		bool depthTest{ true };									// depth test
		DepthFunc depthFunc{ DepthFunc::LESS };					// depth test function

		bool wireframe{ false };								// if we're drawing using wireframe mode

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go diffuse map
		 */
		bool hasDiffuseTexture() const
		{
			return diffuse.map && diffuse.map->handle.glId;
		}

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go normal map
		 */
		bool hasNormalTexture() const
		{
			return normal.map && normal.map->handle.glId;
		}

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go specular map
		 */
		bool hasSpecularTexture() const
		{
			return specular.map && specular.map->handle.glId;
		}
	};
}