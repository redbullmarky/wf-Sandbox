#pragma once
#include "Engine.h"

#include <memory>

namespace Sandbox::Component
{
	struct Material
	{
		unsigned int handle{};

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
	};
}