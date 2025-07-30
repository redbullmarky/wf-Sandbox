#pragma once
#include "Engine.h"

#include <imgui.h>

namespace Squishies
{
	/**
	 * @brief Abusing ImGui's draw system to make up for our own current shortfalls...
	 *
	 * Externally, just: Debug::instance().setCamera(yourcamera) somewhere
	 *
	 * Then use the shape methods statically. @todo confirm if it has to be within ImGui::Begin() or anything, as we might then want to keep a list of things
	 * to draw until we're good to go in future.
	 */
	class Debug
	{
	public:
		static Debug& instance()
		{
			static Debug l_debug;
			return l_debug;
		}

		static void circle(const wf::Vec3& pos, float radius, const wf::Colour& colour = wf::RED, bool fill = false)
		{
			auto& i = instance();
			auto screenPos = i.getScreenPos(pos);
			auto col = colour.denormalised();

			ImDrawList* drawList = ImGui::GetForegroundDrawList();
			if (fill) {
				drawList->AddCircleFilled(screenPos, radius, IM_COL32(col.r, col.g, col.b, col.a));
			}
			else {
				drawList->AddCircle(screenPos, radius, IM_COL32(col.r, col.g, col.b, col.a));
			}
		}

		static void filledCircle(const wf::Vec3& pos, float radius, const wf::Colour& colour = wf::RED)
		{
			circle(pos, radius, colour, true);
		}

		static void line(const wf::Vec3& from, const wf::Vec3& to, float thickness = 2.f, const wf::Colour& colour = wf::RED)
		{
			auto& i = instance();
			auto fromPos = i.getScreenPos(from);
			auto toPos = i.getScreenPos(to);
			auto col = colour.denormalised();

			ImDrawList* drawList = ImGui::GetForegroundDrawList();
			drawList->AddLine(fromPos, toPos, IM_COL32(col.r, col.g, col.b, col.a), thickness);
		}

		static void rect(const wf::Vec3& from, const wf::Vec3& to, float thickness = 2.f, const wf::Colour& colour = wf::RED)
		{
			auto& i = instance();
			auto fromPos = i.getScreenPos(from);
			auto toPos = i.getScreenPos(to);
			auto col = colour.denormalised();

			ImDrawList* drawList = ImGui::GetForegroundDrawList();
			drawList->AddRect(fromPos, toPos, IM_COL32(col.r, col.g, col.b, col.a), 0.f, 0, thickness);
		}

		static void rect(const wf::BoundingBox& box, float thickness = 2.f, const wf::Colour& colour = wf::RED)
		{
			rect(box.min, box.max, thickness, colour);
		}

		void setCamera(wf::component::Camera* camera)
		{
			m_camera = camera;
		}

		ImVec2 getScreenPos(const wf::Vec3& pos)
		{
			glm::vec4 clipSpace = m_camera->getViewProjectionMatrix().matrix * glm::vec4(pos, 1.0f);
			if (clipSpace.w <= 0.0f) return{}; // behind camera

			glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w; // -1 to +1
			glm::vec2 screenPos{};
			screenPos.x = (ndc.x * 0.5f + 0.5f) * wf::getWindow().getWidth();
			screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * wf::getWindow().getHeight();
			return { screenPos.x, screenPos.y };
		}

	private:
		Debug() = default;

	private:
		wf::component::Camera* m_camera;
	};
}