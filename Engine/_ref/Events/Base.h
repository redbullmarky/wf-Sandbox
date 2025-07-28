#pragma once
#include <SDL3/SDL.h>

namespace wf
{
	struct BaseEvent
	{
		virtual ~BaseEvent() = default;
	};

	struct BaseSDLEvent : BaseEvent
	{
		SDL_Event* event;

		BaseSDLEvent(SDL_Event* event) : event(event) {}
	};
}