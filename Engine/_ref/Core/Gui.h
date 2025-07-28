#pragma once
#include <SDL3/SDL.h>

namespace wf
{
	class Window;

	class Gui
	{
	public:
		Gui(Window* window);
		~Gui();

		bool init();
		void shutdown();

		bool handleEvent(const SDL_Event* event);
		bool isInitialised() const;
		bool isFocussed() const;
		bool isRenderReady() const;
		bool beginFrame();
		void endFrame();

		void render();

	private:
		Window* m_window;
		bool m_initialised{ false };
		bool m_inFrame{ false };
		bool m_renderReady{ false };
	};
}