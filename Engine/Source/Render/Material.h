#pragma once
#include "Shader.h"

#include <memory>
#include <vector>

namespace wf
{
	struct Material
	{
		unsigned int handle{};					// internal handle
		std::shared_ptr<Shader> shader;			// shader we're using

		static std::shared_ptr<Material> create()
		{
			return std::shared_ptr<Material>(new Material());
		}

	private:
		Material() = default;
	};
}