#pragma once
#include "Geometry.h"

#include <memory>
#include <vector>

namespace wf
{
	struct Mesh
	{
		unsigned int handle{};

		std::vector<wf::Vertex> vertices;
		std::vector<unsigned int> indices;

		static std::shared_ptr<Mesh> create()
		{
			return std::shared_ptr<Mesh>(new Mesh());
		}

	private:
		Mesh() = default;
	};
}