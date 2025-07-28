#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace wf
{
	struct Shader
	{
		unsigned int handle{ 0 };							// internal GL id
		std::unordered_map<std::string, int> locs;			// shader locations

		static std::shared_ptr<Shader> create()
		{
			return std::shared_ptr<Shader>(new Shader());
		}

		bool isValidLocation(const std::string& loc)
		{
			return locs.contains(loc) && locs[loc] >= 0;
		}

	private:
		Shader() = default;
	};
}