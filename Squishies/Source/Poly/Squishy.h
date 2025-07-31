#pragma once
#include "Engine.h"

#include "Poly.h"

#include <vector>

namespace Squishies
{
	struct Joint
	{
		size_t from;
		size_t to;
		float rest;

		Joint(size_t from, size_t to, float rest) : from(from), to(to), rest(rest) {}
	};

	struct Squishy
	{
		Poly poly;									// the shape
		std::vector<Joint> joints;					// joints between the points
		wf::Colour colour;							// default colour

		Squishy() = default;
		Squishy(const Poly& poly) : poly(poly) {}
		Squishy(const Poly& poly, std::vector<Joint> joints) : poly(poly), joints(joints) {}

		const std::vector<wf::Vec2> getPoints() const;
		const std::vector<Joint> getJoints() const;

		const wf::Vec2 getPoint(size_t index) const;

		std::shared_ptr<wf::Mesh> createMesh();
	};
}