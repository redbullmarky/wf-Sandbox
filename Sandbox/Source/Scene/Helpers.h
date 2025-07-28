#pragma once
#include "Engine.h"

namespace Sandbox
{
	std::shared_ptr<wf::Mesh> createHelloTriangle()
	{
		auto mesh = wf::Mesh::create();
		mesh->vertices.resize(3);
		mesh->vertices[0].position = wf::Vec3{ -0.5f, -0.5f, 0.0f, };
		mesh->vertices[1].position = wf::Vec3{ 0.5f, -0.5f, 0.0f, };
		mesh->vertices[2].position = wf::Vec3{ 0.0f,  0.5f, 0.0f };

		mesh->vertices[0].normal = wf::Vec3{ 0.f, 0.f, 1.f };
		mesh->vertices[1].normal = wf::Vec3{ 0.f, 0.f, 1.f };
		mesh->vertices[2].normal = wf::Vec3{ 0.f, 0.f, 1.f };

		mesh->vertices[0].colour = { 1.f, 0.f, 0.f, 1.f };
		mesh->vertices[1].colour = { 0.f, 1.f, 0.f, 1.f };
		mesh->vertices[2].colour = { 0.f, 0.f, 1.f, 1.f };

		mesh->vertices[0].texcoord = { 0.f, 0.f };
		mesh->vertices[1].texcoord = { 1.f, 0.f };
		mesh->vertices[2].texcoord = { 0.5f, 1.f };

		return mesh;
	}
}
