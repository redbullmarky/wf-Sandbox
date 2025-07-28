#pragma once
#include "Engine.h"

namespace Sandbox
{
	std::shared_ptr<wf::Mesh> createTriangleMesh()
	{
		auto mesh = wf::Mesh::create();

		return mesh;
	}
}