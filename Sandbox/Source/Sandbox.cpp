#include "Sandbox.h"
#include "Engine.h"

#include "Scene/GameScene.h"

#include <imgui.h>
#include <GL/glew.h>

namespace Sandbox
{
	bool Sandbox::init()
	{
		if (!wf::init("Sandbox", 1600, 900)) {
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

	void Sandbox::run()
	{
		//auto rt = wf::wgl::createRenderTarget(1024, 1024);

		while (!wf::shouldClose()) {
			while (wf::isFixedUpdateReady()) {
				m_scene->fixedUpdate(wf::getFixedTime());
			}
			m_scene->update(wf::getDeltaTime());

			if (wf::beginDrawing()) {

				//wf::wgl::bindRenderTarget(rt);

				m_scene->render(wf::getDeltaTime());

				/*wf::wgl::unbindRenderTarget(rt);
				wf::wgl::blitRenderTarget(rt, 0, 0, wf::getWindow().getWidth(), wf::getWindow().getHeight());*/

				if (wf::beginGui()) {
					m_scene->renderGui(wf::getDeltaTime());
					wf::endGui();
				}
				else {
					throw std::runtime_error("balls");
				}

				wf::endDrawing();
			}
			else {
				throw std::runtime_error("oh");
			}

		}

		//wf::wgl::destroyRenderTarget(rt);
	}

	void Sandbox::shutdown()
	{
		m_scene->shutdown();
		wf::shutdownGui();
		wf::shutdown();
	}
}