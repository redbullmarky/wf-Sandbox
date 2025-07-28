#include "pch.h"
#include "Core/Core.h"
#include "Core/Draw.h"
#include "Renderer.h"

namespace wf
{
	void Renderer::clearBackground(const wf::Colour& colour)
	{
		g_gameState.window.clear(colour);
	}

	void Renderer::pushTransform(Mat4 transform)
	{
		if (m_transformStack.size()) {
			transform = m_transformStack.top() * transform;
		}
		m_transformStack.push(transform);
	}

	void Renderer::popTransform()
	{
		m_transformStack.pop();
	}

	Mat4 Renderer::getEffectiveTransform()
	{
		return m_transformStack.size() ? m_transformStack.top() : Mat4{ 1.f };
	}

	void Renderer::draw(Mesh* mesh, IMaterial* material, const Mat4& transform)
	{
		material->use();
		auto shader = material->shader;

		wf::Vec3 lightDir = Vec3{ 0.f, -1.f, 0.f };
		wf::Vec3 viewPos = Vec3{ 1.f, 0.f, 5.f };
		wf::Mat4 vp = Mat4{ 1.f };
		wf::Mat4 mvp(vp * getEffectiveTransform() * transform);

		wf::setShaderUniform(
			*shader,
			shader->locs["lightDir"],
			lightDir
		);
		wf::setShaderUniform(
			*shader,
			shader->locs["viewPos"],
			viewPos
		);
		wf::setShaderUniform(
			*shader,
			shader->locs["matModel"],
			transform
		);
		wf::setShaderUniform(
			*shader,
			shader->locs["mvp"],
			mvp
		);
		wf::setShaderUniform(
			*shader,
			shader->locs["lightColour"],
			wf::WHITE
		);
		wf::setShaderUniform(
			*shader,
			shader->locs["ambientLevel"],
			0.5f
		);

		setCullMode(material->cullMode);
		setBlendMode(material->blendMode);
		setDepthSettings(material->depthMask, material->depthTest, material->depthFunc);
		wf::drawMesh(*mesh, material->wireframe);
	}

	void Renderer::render(Scene* scene, Camera* camera)
	{
		// @todo render a whole scene a-la threejs
	}

	void Renderer::reset()
	{
		// @todo clean up, etc
	}
}