#pragma once
#include "Engine.h"

#include <memory>

namespace Sandbox::Component
{
	struct Material
	{
		std::shared_ptr<wf::Shader> shader;

		struct Diffuse
		{
			std::shared_ptr<wf::Texture> map;
			wf::Colour colour{ wf::WHITE };
		} diffuse;

		struct Normal
		{
			std::shared_ptr<wf::Texture> map;
			float strength{ 1.f };
		} normal;

		struct Specular
		{
			std::shared_ptr<wf::Texture> map;
			wf::Colour colour{ wf::WHITE };
			float shininess{ 32.f };
			float intensity{ 0.f };
		} specular;

		bool hasDiffuseTexture() const
		{
			return diffuse.map && diffuse.map->handle;
		}

		bool hasNormalTexture() const
		{
			return normal.map && normal.map->handle;
		}

		bool hasSpecularTexture() const
		{
			return specular.map && specular.map->handle;
		}
	};
}