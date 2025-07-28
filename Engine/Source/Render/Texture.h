#pragma once
#include <memory>
#include <vector>

namespace wf
{
	struct Texture
	{
		unsigned int handle{};

		static std::shared_ptr<Texture> create()
		{
			return std::shared_ptr<Texture>(new Texture());
		}

	private:
		Texture() = default;
	};
}