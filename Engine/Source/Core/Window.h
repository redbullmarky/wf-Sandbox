#pragma once
#include "Misc/Colour.h"

#include <SDL3/SDL.h>

namespace wf
{
	class Window
	{
	public:
		Window() = default;
		~Window() = default;

		bool open(const char* title, int width, int height, int flags);
		void close();

		float getAspectRatio() const;

		void clear(const Colour& colour);
		void swapBuffers();

		SDL_Window* getHandle();
		SDL_GLContext getContext() const;

	private:
		// internals
		SDL_Window* m_window{ nullptr };
		SDL_GLContext m_glContext{};
	};
}
