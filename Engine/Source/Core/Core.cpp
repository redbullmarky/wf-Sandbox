#include "pch.h"
#include "Core.h"

#include "GL.h"
#include "Gui.h"
#include "Render/Texture.h"

#include <SDL3/SDL.h>

namespace wf
{
	State g_gameState;

	bool init(const char* title, int width, int height, int flags)
	{
		return g_gameState.window.open(title, width, height, flags);
	}

	void shutdown()
	{
		g_gameState.window.close();
	}

	void close()
	{
		g_gameState.shouldClose = true;
	}

	bool shouldClose()
	{
		if (!g_gameState.shouldClose && !g_gameState.paused) {
			g_gameState.timer.tick();
			pollEvents();
		}

		return g_gameState.shouldClose;
	}

	float getAspectRatio()
	{
		return g_gameState.window.getAspectRatio();
	}

	Window& getWindow()
	{
		return g_gameState.window;
	}

	bool beginDrawing()
	{
		// @todo return a renderer when we get further along
		return true;
	}

	void endDrawing()
	{
		if (g_gameState.guiHandler.isRenderReady()) {
			g_gameState.guiHandler.render();
		}
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

	void createTimer(float duration, TimerCallback callback, bool autoRenew, int renewCount)
	{
		g_gameState.timer.createTimer(duration, callback, autoRenew, renewCount);
	}

	void pauseTimers(bool pause)
	{
		g_gameState.timer.pauseTimers(pause);
	}

	void clearTimers()
	{
		g_gameState.timer.clearTimers();
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

	void showCursor(bool show)
	{
		if (show) {
			SDL_ShowCursor();
		}
		else {
			SDL_HideCursor();
		}
	}

	bool isCursorVisible()
	{
		return SDL_CursorVisible();
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

	Vec3 getMouseWorldPosition(const component::Camera& camera)
	{
		Vec2 mousePos = getMousePosition();
		Vec2 screenSize = getWindow().getSize();

		Vec2 ndc{
			(mousePos.x / screenSize.x) * 2.0f - 1.0f,
			1.0f - (mousePos.y / screenSize.y) * 2.0f
		};

		Vec4 clipNear(ndc.x, ndc.y, -1.0f, 1.0f);
		Vec4 clipFar(ndc.x, ndc.y, 1.0f, 1.0f);

		Mat4 invVP = Mat4(glm::inverse(camera.getViewProjectionMatrix().matrix));

		Vec4 worldNear = invVP.matrix * clipNear;
		Vec4 worldFar = invVP.matrix * clipFar;

		worldNear /= worldNear.w;
		worldFar /= worldFar.w;

		Vec3 dir = glm::normalize(Vec3(worldFar - worldNear));

		// intersect ray with Z=0 plane
		float t = -worldNear.z / dir.z;
		return Vec3(worldNear) + dir * t;
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

	std::shared_ptr<Texture> loadTexture(const char* filename)
	{
		auto texture = wf::Texture::create();
		texture->handle = wf::wgl::loadTexture(filename);
		return texture;
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