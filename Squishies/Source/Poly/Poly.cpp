#include "Poly.h"

namespace Squishies
{
	Poly::Poly(std::vector<wf::Vec2> points, wf::Vec2 origin)
	{
	}

	wf::Vec2 Poly::getCenter() const
	{
		return wf::Vec2();
	}

	Poly& Poly::transform(const wf::Transform& transform)
	{
		return *this;
	}

	Poly& Poly::translate(const wf::Vec2& offset)
	{
		wf::Transform trans(wf::Vec3(repos));

		return *this;
	}

	Poly& Poly::rotate(float angleDegrees)
	{
		wf::Transform trans(wf::Vec3(offset));

		// TODO: insert return statement here
		return *this;
	}

	Poly& Poly::scale(float scale)
	{
		// TODO: insert return statement here
		return *this;
	}
}