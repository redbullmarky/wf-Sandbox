#pragma once
#include "Engine.h"

#include <imgui.h>
#include <memory>
#include <vector>

namespace Squishies
{
	/**
	 * @brief Abusing ImGui's draw system to make up for our own current shortfalls...
	 *
	 * Externally, just: Debug::instance().setCamera(yourcamera) somewhere
	 *
	 * Then use the shape methods statically. Calls are batched so that they can be unleashed when ImGui's ready to accept them...
	 */
	class Debug
	{
	public:
		struct IShape
		{
			wf::Colour colour;

			IShape(wf::Colour colour) : colour(colour) {}
			virtual ~IShape() = default;

			virtual void draw() = 0;
		};

		struct Circle : public IShape
		{
			wf::Vec3 pos;
			float radius;
			bool fill;

			Circle(wf::Vec3 pos, float radius, wf::Colour colour, bool fill) : IShape(colour), pos(pos), radius(radius), fill(fill) {}

			virtual void draw() override
			{
				auto& i = Debug::instance();
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
		};

		struct Line : public IShape
		{
			wf::Vec3 from;
			wf::Vec3 to;
			float thickness{ 1.f };

			Line(wf::Vec3 from, wf::Vec3 to, float thickness = 1.f, wf::Colour colour = wf::RED) : IShape(colour), from(from), to(to), thickness(thickness) {}

			virtual void draw() override
			{
				auto& i = Debug::instance();
				auto fromPos = i.getScreenPos(from);
				auto toPos = i.getScreenPos(to);
				auto col = colour.denormalised();

				ImDrawList* drawList = ImGui::GetForegroundDrawList();
				drawList->AddLine(fromPos, toPos, IM_COL32(col.r, col.g, col.b, col.a), thickness);
			}
		};

		struct Rect : public Line
		{
			using Line::Line;

			virtual void draw() override
			{
				auto& i = Debug::instance();
				auto fromPos = i.getScreenPos(from);
				auto toPos = i.getScreenPos(to);
				auto col = colour.denormalised();

				ImDrawList* drawList = ImGui::GetForegroundDrawList();
				drawList->AddRect(fromPos, toPos, IM_COL32(col.r, col.g, col.b, col.a), 0.f, 0, thickness);
			}
		};

		static Debug& instance()
		{
			static Debug l_debug;
			return l_debug;
		}

		static void render()
		{
			instance().renderAll();
		}

		static void circle(const wf::Vec3& pos, float radius, const wf::Colour& colour = wf::RED, bool fill = false)
		{
			instance().addShape(std::make_shared<Circle>(pos, radius, colour, fill));
		}

		static void filledCircle(const wf::Vec3& pos, float radius, const wf::Colour& colour = wf::RED)
		{
			instance().addShape(std::make_shared<Circle>(pos, radius, colour, true));
		}

		static void line(const wf::Vec3& from, const wf::Vec3& to, float thickness = 2.f, const wf::Colour& colour = wf::RED)
		{
			instance().addShape(std::make_shared<Line>(from, to, thickness, colour));
		}

		static void rect(const wf::Vec3& from, const wf::Vec3& to, float thickness = 2.f, const wf::Colour& colour = wf::RED)
		{
			instance().addShape(std::make_shared<Rect>(from, to, thickness, colour));
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

		void addShape(std::shared_ptr<IShape> shape)
		{
			m_shapes.push_back(shape);
		}

	private:
		Debug() = default;

		void renderAll()
		{
			for (auto& shape : m_shapes) {
				shape->draw();
			}
			m_shapes.clear();
		}

	private:
		wf::component::Camera* m_camera{ nullptr };

		std::vector<std::shared_ptr<IShape>> m_shapes;
	};
}