#pragma once
#include "Engine.h"

#include "Event/SplitSquishyEvent.h"

namespace Squishies
{
	/**
	 * @brief Splitting and combining softbodies
	 */
	class CharacterDamageSystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;

	private:
		void split(event::SplitSquishy& event);
	};
}


