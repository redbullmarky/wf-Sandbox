#pragma once
#include "Core/GL.h"

#include <string>
#include <unordered_map>

namespace wf
{
	struct Shader
	{
		wgl::ShaderHandle handle;							// internal GL 
		std::unordered_map<std::string, int> locs;			// shader locations

		bool isValidLocation(const std::string& loc) const
		{
			return locs.contains(loc) && locs.at(loc) >= 0;
		}

		int location(const std::string& loc) const
		{
			return locs.at(loc);
		}
	};
}