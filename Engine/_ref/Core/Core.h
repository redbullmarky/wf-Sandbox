#pragma once
#include "Gui.h"
#include "Input.h"
#include "Shader.h"
#include "Timer.h"
#include "Window.h"
#include "Math/Math.h"
#include "Render/Renderer.h"

namespace wf
{
	struct State
	{
		// config
		KeyCode quitKey = KEY_ESCAPE;

		// state
		bool shouldClose{ false };
		bool paused{ false };

		Gui guiHandler;
		Input inputHandler;
		Renderer renderer;
		Timer timer;
		Window window;

		State() : guiHandler(&window) {}
	};

	extern State g_gameState;

	bool init(const char* title, int width, int height, int flags = 0);
	void shutdown();

	void close();
	bool shouldClose();

	float getAspectRatio();

	Renderer& beginDrawing();
	void endDrawing();

	bool initGui();
	void shutdownGui();
	bool beginGui();
	void endGui();
	bool isGuiFocussed();

	bool isFixedUpdateReady();
	float getDeltaTime();
	float getFixedTime();
	float getFps();

	bool isKeyPressed(KeyCode key);
	bool isKeyReleased(KeyCode key);
	bool isKeyHeld(KeyCode key);

	bool isMouseButtonPressed(int button);
	bool isMouseButtonReleased(int button);
	bool isMouseButtonHeld(int button);

	Vec2 getMousePosition();
	Vec2 getMouseWheel();
	Vec2 getMouseDelta();
}

namespace {
	static void tick();
	static void pollEvents();
}
