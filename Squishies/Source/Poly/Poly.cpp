#include "Poly.h"

namespace Squishies
{
	Poly::Poly(std::vector<wf::Vec2> points) : points(points)
	{
	}

	wf::Vec2 Poly::getCenter() const
	{
		wf::Vec2 center{};
		const float invPC = 1.f / points.size();

		for (auto& pt : points) {
			center += pt;
		}

		center *= invPC;

		return center;
	}

	Poly& Poly::transform(const wf::Transform& transform)
	{
		for (auto& p : points) {
			auto tp = transform.getWorldPosition(wf::Vec3(p, 0.f));
			p = { tp.x, tp.y };
		}
		return *this;
	}

	Poly Poly::getTransformed(const wf::Transform& transform) const
	{
		Poly ret(*this);
		ret.transform(transform);
		return ret;
	}

	Poly& Poly::translate(const wf::Vec2& offset)
	{
		transform(wf::Transform::t(wf::Vec3(offset, 0.f)));
		return *this;
	}

	Poly& Poly::rotate(float angleDegrees)
	{
		transform(wf::Transform::r(wf::Vec3(0.f, 0.f, angleDegrees))); // 2d rotation is essentialy around the Z axis.
		return *this;
	}

	Poly& Poly::scale(float scale)
	{
		transform(wf::Transform::s(scale));
		return *this;
	}
}