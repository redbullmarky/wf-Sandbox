#include "pch.h"
#include "Core.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include "Gui.h"

namespace wf
{
	State g_gameState;

	bool init(const char* title, int width, int height, int flags)
	{
		return g_gameState.window.open(title, width, height, flags);
	}

	void shutdown()
	{
		destroyShaderRegistry();
		g_gameState.window.close();
	}

	void close()
	{
		g_gameState.shouldClose = true;
	}

	bool shouldClose()
	{
		if (!g_gameState.shouldClose) {
			g_gameState.timer.tick();
			pollEvents();
		}

		return g_gameState.shouldClose;
	}

	float getAspectRatio()
	{
		return g_gameState.window.getAspectRatio();
	}

	Renderer& beginDrawing()
	{
		return g_gameState.renderer;
	}

	void endDrawing()
	{
		// @todo submit the renderer for actual drawing
		if (g_gameState.guiHandler.isInitialised()) {
			g_gameState.guiHandler.render();
		}

		g_gameState.renderer.reset();
		g_gameState.window.swapBuffers();
	}

	bool initGui()
	{
		return g_gameState.guiHandler.init();
	}

	void shutdownGui()
	{
		g_gameState.guiHandler.shutdown();
	}

	bool beginGui()
	{
		return g_gameState.guiHandler.beginFrame();
	}

	void endGui()
	{
		g_gameState.guiHandler.endFrame();
	}

	bool isGuiFocussed()
	{
		return g_gameState.guiHandler.isFocussed();
	}

	bool isFixedUpdateReady()
	{
		return g_gameState.timer.isFixedUpdateReady();
	}

	float getDeltaTime()
	{
		return g_gameState.timer.getDeltaTime();
	}

	float getFixedTime()
	{
		return g_gameState.timer.getFixedTime();
	}

	float getFps()
	{
		return g_gameState.timer.getFps();
	}

	bool isKeyPressed(KeyCode key)
	{
		return g_gameState.inputHandler.isKeyPressed(key);
	}

	bool isKeyReleased(KeyCode key)
	{
		return g_gameState.inputHandler.isKeyReleased(key);
	}

	bool isKeyHeld(KeyCode key)
	{
		return g_gameState.inputHandler.isKeyHeld(key);
	}

	bool isMouseButtonPressed(int button)
	{
		return g_gameState.inputHandler.isMouseButtonPressed(button);
	}

	bool isMouseButtonReleased(int button)
	{
		return g_gameState.inputHandler.isMouseButtonReleased(button);
	}

	bool isMouseButtonHeld(int button)
	{
		return g_gameState.inputHandler.isMouseButtonHeld(button);
	}

	Vec2 getMousePosition()
	{
		return g_gameState.inputHandler.getMousePosition();
	}

	Vec2 getMouseWheel()
	{
		return g_gameState.inputHandler.getMouseWheel();
	}

	Vec2 getMouseDelta()
	{
		return g_gameState.inputHandler.getMouseDelta();
	}
}

namespace
{
	static void pollEvents()
	{
		wf::g_gameState.inputHandler.refresh();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			wf::g_gameState.guiHandler.handleEvent(&e);

			switch (e.type) {
			case SDL_EVENT_QUIT:
				wf::close();
				break;

			case SDL_EVENT_KEY_DOWN:
				if (e.key.scancode == wf::g_gameState.quitKey) {
					wf::close();
					return;
				}
				[[fallthrough]];
			case SDL_EVENT_KEY_UP:
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_MOTION:
			case SDL_EVENT_MOUSE_WHEEL:
				wf::g_gameState.inputHandler.processEvent(&e);
				break;
			}
		}
	}
}