#include "pch.h"
#include "Draw.h"
#include "Geometry.h"

namespace wf
{
	void setCullMode(CullMode mode)
	{
		switch (mode)
		{
		case CullMode::NONE:
			glDisable(GL_CULL_FACE);
			break;
		case CullMode::BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CullMode::FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		}
	}

	void setBlendMode(BlendMode mode)
	{
		switch (mode)
		{
		case BlendMode::OPAQUE:
			glDisable(GL_BLEND);
			break;
		case BlendMode::ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::ADDITIVE:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case BlendMode::MULTIPLY:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		}
	}

	void setDepthSettings(bool depthMask, bool depthTest, DepthFunc depthFunc)
	{
		enableDepthMask(depthMask);
		enableDepthTest(depthTest);

		switch (depthFunc)
		{

		case DepthFunc::EQUAL:
			glDepthFunc(GL_EQUAL);
			break;
		case DepthFunc::LEQUAL:
			glDepthFunc(GL_LEQUAL);
			break;
		case DepthFunc::GREATER:
			glDepthFunc(GL_GREATER);
			break;
		case DepthFunc::GEQUAL:
			glDepthFunc(GL_GEQUAL);
			break;
		case DepthFunc::ALWAYS:
			glDepthFunc(GL_ALWAYS);
			break;
		case DepthFunc::NEVER:
			glDepthFunc(GL_NEVER);
			break;
		case DepthFunc::LESS:
		default:
			glDepthFunc(GL_LESS);
			break;
		}
	}

	void setWireframeMode(bool enabled)
	{
		glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
	}

	void enableDepthTest(bool enable)
	{
		if (enable) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	void enableDepthMask(bool enable)
	{
		glDepthMask(enable ? GL_TRUE : GL_FALSE);
	}

	void drawMesh(const Mesh& mesh, bool wireframe)
	{
		bindMesh(mesh);

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (mesh.indices.size())
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertices.size()));

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		unbindMesh(mesh);
	}
}