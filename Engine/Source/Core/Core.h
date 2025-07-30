#pragma once
#include "Gui.h"
#include "Input.h"
#include "Math/Math.h"
#include "Scene/Component/Camera.h"
#include "Timer.h"
#include "Window.h"

#include <memory>

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
		Timer timer;
		Window window;

		State() : guiHandler(&window) {}
	};

	struct Texture;

	extern State g_gameState;

	bool init(const char* title, int width, int height, int flags = 0);
	void shutdown();

	void close();
	bool shouldClose();

	float getAspectRatio();
	Window& getWindow();

	bool beginDrawing();
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

	void showCursor(bool show = true);
	bool isCursorVisible();
	bool isMouseButtonPressed(int button);
	bool isMouseButtonReleased(int button);
	bool isMouseButtonHeld(int button);

	Vec3 getMouseWorldPosition(const component::Camera& camera);
	Vec2 getMousePosition();
	Vec2 getMouseWheel();
	Vec2 getMouseDelta();

	[[nodiscard]] std::shared_ptr<Texture> loadTexture(const char* filename);
}

namespace {
	static void tick();
	static void pollEvents();
}
