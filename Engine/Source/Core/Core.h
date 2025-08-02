#pragma once
#include "Gui.h"
#include "Input.h"
#include "Math/Math.h"
#include "Render/Renderer.h"
#include "ResourceManager.h"
#include "Scene/Component/CameraComponent.h"
#include "Timer.h"
#include "Window.h"

#include <memory>
#include <stack>

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
		ResourceManager resourceManager;
		Timer timer;
		Window window;

		std::stack<std::shared_ptr<Renderer>> renderStack;

		State() : guiHandler(&window) {}
	};

	extern State g_gameState;

	bool init(const char* title, int width, int height, int flags = 0);
	void shutdown();

	ResourceManager& getResourceManager();

	void close();
	bool shouldClose();

	float getAspectRatio();
	Window& getWindow();

	Renderer* beginDrawing();
	Renderer* getCurrentRenderer();
	void endDrawing();

	bool initGui();
	void shutdownGui();
	bool beginGui();
	void endGui();
	bool isGuiFocussed();

	bool isFixedUpdateReady();
	float getDeltaTime();
	float getFixedTimestep();
	void setFixedTimestep(float timestep = 1.f / 60.f);
	float getFps();
	void createTimer(float duration, TimerCallback callback, bool autoRenew = false, int renewCount = -1);
	void pauseTimers(bool pause = true);
	void clearTimers();

	bool isKeyPressed(KeyCode key);
	bool isKeyReleased(KeyCode key);
	bool isKeyHeld(KeyCode key);

	void showCursor(bool show = true);
	bool isCursorVisible();
	bool isMouseButtonPressed(int button);
	bool isMouseButtonReleased(int button);
	bool isMouseButtonHeld(int button);

	Vec3 getMouseWorldPosition(const CameraComponent& camera);
	Vec2 getMousePosition();
	Vec2 getMouseWheel();
	Vec2 getMouseDelta();
}

namespace {
	static void pollEvents();
}
