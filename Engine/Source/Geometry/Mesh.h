#pragma once
#include "Geometry.h"

#include <memory>
#include <vector>

namespace wf
{
	struct Mesh
	{
		unsigned int handle{};							// internal handle to associate with the GPU data

		std::vector<wf::Vertex> vertices;				// vertices
		std::vector<unsigned int> indices;				// indices for the triangles

		bool isDynamic{ false };						// whether the mesh data is static or can be updated
		bool needsUpdate{ true };						// if the mesh data is dirty and needs re-uploading
		bool autoUpdate{ true };						// if we want to automatically update the data without having to set the needsUpdate flag
		bool wireframe{ false };						// render as a wireframe

		static std::shared_ptr<Mesh> create()
		{
			return std::shared_ptr<Mesh>(new Mesh());
		}

	private:
		Mesh() = default;
	};
}