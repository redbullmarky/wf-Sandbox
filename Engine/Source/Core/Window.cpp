#include "pch.h"
#include "Window.h"

#include "GL.h"

#include <SDL3/SDL.h>

namespace wf
{
	bool Window::open(const char* title, int width, int height, int flags)
	{
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
			return false;
		}

		m_window = SDL_CreateWindow(title, width, height, flags | SDL_WINDOW_OPENGL);
		if (!m_window) {
			SDL_Log("SDL_CreateWindow() failed: %s", SDL_GetError());
			return false;
		}

		m_glContext = SDL_GL_CreateContext(m_window);
		if (!m_glContext) {
			SDL_Log("SDL_GL_CreateContext() failed: %s", SDL_GetError());
			close();
			return false;
		}

		if (!wgl::init()) {
			SDL_Log("SDL_GL_CreateContext() failed: %s", SDL_GetError());
			close();
		}

		wgl::setViewport(width, height);
		wgl::enableDepthTest();

		SDL_GL_SetSwapInterval(1); // enables vsync

		return true;
	}

	void Window::close()
	{
		if (m_glContext) {
			SDL_GL_DestroyContext(m_glContext);
			m_glContext = 0;
		}

		if (m_window) {
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}

		SDL_Quit();
	}

	Vec2 Window::getSize() const
	{
		int w{}, h{};
		SDL_GetWindowSize(m_window, &w, &h);
		return { (float)w, (float)h };
	}

	int Window::getWidth() const
	{
		return (int)getSize().x;
	}

	int Window::getHeight() const
	{
		return (int)getSize().y;
	}

	float Window::getAspectRatio() const
	{
		int x{}, y{};
		SDL_GetWindowSize(m_window, &x, &y);
		return (float)x / (float)y;
	}

	void Window::clear(const Colour& colour)
	{
		wgl::clearColour(colour);
	}

	void Window::swapBuffers()
	{
		SDL_GL_SwapWindow(m_window);
	}

	SDL_Window* Window::getHandle()
	{
		return m_window;
	}

	SDL_GLContext Window::getContext() const
	{
		return m_glContext;
	}
}