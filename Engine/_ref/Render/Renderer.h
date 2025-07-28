#pragma once
#include "Core/Geometry.h"
#include "Material.h"
#include "Math/Math.h"

#include <stack>
#include <vector>

// @todo - currently "immediate" - need to batch/queue
// we could do this by making things like ImmediateRenderer or SceneRenderer or DeferreredRenderer etc and we're good.

namespace wf
{
	struct Camera;
	class Scene;

	struct RenderTarget {}; // @todo

	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

		void clearBackground(const wf::Colour& colour);

		void pushTransform(Mat4 transform);
		void popTransform();
		Mat4 getEffectiveTransform();

		void draw(Mesh* mesh, IMaterial* material, const Mat4& transform = { 1.f });
		void render(Scene* scene, Camera* camera);

		void setRenderTarget(RenderTarget* renderTarget) {}
		void clearRenderTarget() {}

		void reset();

	private:
		std::stack<Mat4> m_transformStack;
	};
}