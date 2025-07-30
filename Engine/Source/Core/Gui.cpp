#include "pch.h"
#include "Gui.h"

#include "Window.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

namespace wf
{
	Gui::Gui(Window* window) : m_window(window)
	{
	}

	Gui::~Gui()
	{
		shutdown();
	}

	bool Gui::init()
	{
		if (!m_initialised) {
			ImGui::CreateContext(); // create the context
			ImGui_ImplSDL3_InitForOpenGL(m_window->getHandle(), m_window->getContext());
			ImGui_ImplOpenGL3_Init();

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

			m_initialised = true;
		}
		return m_initialised;
	}

	void Gui::shutdown()
	{
		if (m_initialised) {
			ImGui_ImplSDL3_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext();
			m_initialised = false;
		}
	}

	bool Gui::isInitialised() const
	{
		return m_initialised;
	}

	bool Gui::isFocussed() const
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}

	bool Gui::isRenderReady() const
	{
		return m_renderReady;
	}

	bool Gui::handleEvent(const SDL_Event* event) const
	{
		if (!m_initialised) return false;
		return ImGui_ImplSDL3_ProcessEvent(event);
	}

	bool Gui::beginFrame()
	{
		if (!m_initialised) throw std::runtime_error("wf::Gui not initialised");
		if (m_inFrame) throw std::runtime_error("wf::Gui::beginFrame(): Frame already active. Did you forget to endFrame()?");

		ImGui_ImplSDL3_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode);

		// allow custom control of mouse cursor & visibility, else it's just ignored
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		m_inFrame = true;
		m_renderReady = false;
		return true;
	}

	void Gui::endFrame()
	{
		if (!m_inFrame) throw std::runtime_error("wf::Gui::endFrame(): Cannot end frame that wasn't started");
		m_inFrame = false;
		m_renderReady = true;
	}

	void Gui::render()
	{
		if (!m_initialised) throw std::runtime_error("wf::Gui not initialised");
		if (!m_renderReady) return;

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		m_renderReady = false;
	}
}