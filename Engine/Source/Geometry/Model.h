#pragma once

namespace wf
{
	struct Model
	{
		static std::shared_ptr<Model> load()
		{
			// @todo
			return create();
		}

		static std::shared_ptr<Model> create()
		{
			return std::shared_ptr<Model>(new Model());
		}

	private:
		Model() = default;
	};
}