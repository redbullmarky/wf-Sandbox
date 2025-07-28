#pragma once
#include "Mesh/Mesh.h"
#include "Render/Material.h"

namespace wf
{
	void setCullMode(CullMode mode);
	void setBlendMode(BlendMode mode);
	void setDepthSettings(bool depthMask = true, bool depthTest = true, DepthFunc depthFunc = DepthFunc::LESS);
	void setWireframeMode(bool enabled);

	void enableDepthTest(bool enable = true);
	void enableDepthMask(bool enable = true);

	void drawMesh(const Mesh& mesh, bool wireframe = false);
}