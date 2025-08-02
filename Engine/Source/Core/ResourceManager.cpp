#include "pch.h"
#include "ResourceManager.h"

namespace wf
{
	void wf::ResourceManager::shutdown()
	{
		// we'll just clear; registrants will listen to events and act accordingly to clean up
		clear();
	}
}