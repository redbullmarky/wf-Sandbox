#pragma once
#include "Core/GL.h"
#include "Misc/Colour.h"
#include "Render/Shader.h"
#include "Render/Texture.h"

#include <memory>

namespace wf::component
{
	struct Material
	{
		std::shared_ptr<Shader> shader;

		struct Diffuse											// diffuse settings
		{
			std::shared_ptr<Texture> map;						// diffuse texture
			Colour colour{ WHITE };							// base diffuse colour
		} diffuse;

		struct Normal											// normal map settings
		{
			std::shared_ptr<Texture> map;						// normal map
			float strength{ 1.f };									// strength for applying map
		} normal;

		struct Specular											// specular settings
		{
			std::shared_ptr<Texture> map;						// specular map
			Colour colour{ WHITE };							// specular colour
			float shininess{ 32.f };								// how shiny
			float intensity{ 0.f };									// level of specular. defaults to bland.
		} specular;

		struct Shadow											// shadow map (internals, @todo temporary to get it working)
		{
			wgl::RenderTargetHandle* map{ nullptr };				// shadow map
			bool shadowPass{ false };								// if we're creating the shadows (true) or using them.
		} shadow;

		wgl::BlendMode blendMode = wgl::BlendMode::OPAQUE;				// blend mode
		wgl::CullMode cullMode = wgl::CullMode::BACK;						// cull mode
		bool depthMask{ true };									// depth mask
		bool depthTest{ true };									// depth test
		wgl::DepthFunc depthFunc{ wgl::DepthFunc::LESS };					// depth test function

		bool visible{ true };									// current visibility state
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

		/**
		 * @brief Do we have a shadow map we should apply?
		 */
		bool hasShadowMap() const
		{
			return !shadow.shadowPass && shadow.map && shadow.map->fbo && shadow.map->depthTexture.glId;
		}
	};
}