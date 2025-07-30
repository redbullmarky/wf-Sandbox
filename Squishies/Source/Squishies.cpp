#include "Squishies.h"
#include "Engine.h"

#include "Scene/GameScene.h"
#include "Scene/TestScene.h"

#include <SDL3/SDL.h>

namespace Squishies
{
	bool Squishies::init()
	{
		if (!wf::init("Squishies", 1600, 900)) {
			return false;
		}

		wf::initGui();

		m_scene = std::make_shared<GameScene>();
		if (!m_scene->init()) {
			return false;
		}

		m_scene->setup();


		return true;
	}

	void Squishies::run()
	{
		auto cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);

		while (!wf::shouldClose()) {
			SDL_SetCursor(cursor);

			while (wf::isFixedUpdateReady()) {
				m_scene->fixedUpdate(wf::getFixedTime());
			}

			m_scene->update(wf::getDeltaTime());

			if (wf::beginDrawing()) {
				m_scene->render(wf::getDeltaTime());

				if (wf::beginGui()) {
					m_scene->renderGui(wf::getDeltaTime());
					wf::endGui();
				}

				wf::endDrawing();
			}
		}

		SDL_DestroyCursor(cursor);
	}

	void Squishies::shutdown()
	{
		m_scene->shutdown();
		wf::shutdownGui();
		wf::shutdown();
	}
}