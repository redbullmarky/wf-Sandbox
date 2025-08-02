#pragma once
#include "Core/GL.h"
#include "MaterialTraits.h"
#include "Math/Math.h"
#include "Misc/Colour.h"
#include "Shader.h"
#include "Texture.h"

namespace wf
{
	struct Material :
		public TransformTrait,
		public LightingTrait,
		public CameraTrait,
		public DiffuseTrait,
		public NormalTrait,
		public SpecularTrait,
		public ShadowRecieverTrait
	{
		Shader shader;

		void bind(RenderContext& context, const Transform& transform) const
		{
			wgl::useShader(shader.handle);
			TransformTrait::bind(shader, context, transform);
			LightingTrait::bind(shader, context, transform);
			CameraTrait::bind(shader, context, transform);
			DiffuseTrait::bind(shader, context, transform);
			NormalTrait::bind(shader, context, transform);
			SpecularTrait::bind(shader, context, transform);
			ShadowRecieverTrait::bind(shader, context, transform);

			wgl::setCullMode(cullMode);
			wgl::setBlendMode(blendMode);
			wgl::enableDepthTest(depthTest); // @todo apparently only supposed to be done once unless necessary according to gpt.
			wgl::enableDepthMask(depthMask);
			wgl::setDepthFunc(depthFunc);
		}

		wgl::BlendMode blendMode = wgl::BlendMode::OPAQUE;		// blend mode
		wgl::CullMode cullMode = wgl::CullMode::BACK;			// cull mode
		bool depthMask{ true };									// depth mask
		bool depthTest{ true };									// depth test
		wgl::DepthFunc depthFunc{ wgl::DepthFunc::LESS };		// depth test function

		bool visible{ true };									// current visibility state
		bool wireframe{ false };								// if we're drawing using wireframe mode

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go diffuse map
		 */
		bool hasDiffuseTexture() const
		{
			return diffuse.map.handle.glId;
		}

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go normal map
		 */
		bool hasNormalTexture() const
		{
			return normal.map.handle.glId;
		}

		/**
		 * @brief Check the presence of the map as well as internal handle for a valid, ready-to go specular map
		 */
		bool hasSpecularTexture() const
		{
			return specular.map.handle.glId;
		}

		/**
		 * @brief Do we have a shadow map we should apply?
		 */
		bool hasShadowMap() const
		{
			return !shadow.shadowPass && shadow.map.fbo && shadow.map.depthTexture.glId;
		}
	};
}