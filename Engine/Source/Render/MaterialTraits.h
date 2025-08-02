#pragma once
#include "Math/Math.h"
#include "Misc/Colour.h"
#include "Shader.h"
#include "Texture.h"

namespace wf
{
	struct RenderContext;

	struct DiffuseTrait
	{
		struct Diffuse
		{
			Texture map;										// diffuse texture
			Colour colour{ WHITE };								// base diffuse colour
		} diffuse;

		bool hasMap() const;
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct NormalTrait
	{
		struct Normal
		{
			Texture map;										// normal texture
			float strength{ 1.f };								// strength for applying map
		} normal;

		bool hasMap() const;
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct SpecularTrait
	{
		struct Specular
		{
			Texture map;										// specular texture
			Colour colour{ WHITE };								// specular colour
			float shininess{ 32.f };							// how shiny
			float intensity{ 0.f };								// level of specular. defaults to bland.
		} specular;

		bool hasMap() const;
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct LightingTrait
	{
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct CameraTrait
	{
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct ShadowRecieverTrait
	{
		struct Shadow
		{
			wgl::RenderTargetHandle map;						// shadow map
			bool shadowPass{ false };							// if we're creating the shadows (true) or using them.
		} shadow;

		bool hasMap() const;
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};

	struct TransformTrait
	{
		void bind(const Shader& shader, const RenderContext& renderContext, const Transform& transform) const;
	};
}
