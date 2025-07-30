#include "Collider.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Squishies
{
	void Collider::setup(float penetrationThreshold, float elasticity, float friction)
	{
		m_penetrationThreshold = penetrationThreshold;
		m_elasticity = elasticity;
		m_friction = friction;
	}

	void Collider::reset()
	{
		m_collisions.clear();
	}

	// @todo initial checks around whether the objects are the same, or both fixed, or not collidable, etc - things we'll want configured later in our
	//		bodies.
	bool Collider::check(Component::Squishy& obj1, Component::Squishy& obj2)
	{
		// bitmask check..
		if (!obj1.bitFields.same(obj2.bitFields)) {
			return false;
		}

		// bounding boxes collide at least?
		if (!obj1.boundingBox.intersects(obj2.boundingBox)) {
			return false;
		}

		//// @todo temporary testing "squish" mode - the bb y would be tiny...
		//if (
		//	(obj1.boundingBox.GetSize().y < 0.2f && !obj2.isFixed && !obj2.isKinematic) ||
		//	(obj2.boundingBox.GetSize().y < 0.2f && !obj1.isFixed && !obj1.isKinematic)
		//	) {
		//	return false;
		//}

		////now we can do some more involved checks.
		size_t bApmCount = obj1.points.size();
		size_t bBpmCount = obj2.points.size();
		auto boxB = obj2.boundingBox;

		Collision infoAway;
		Collision infoSame;
		bool hasCollisions = false;

		for (size_t i = 0; i < bApmCount; i++) {
			wf::Vec2 pt = obj1.points[i].position;

			// check if the point is even inside the other shape. a simple bb check, then a poly check if necessary.
			if (!boxB.contains(wf::Vec3{ pt, 0.f })) continue;

			if (!checkCollisionPoint(pt, obj2.points)) continue;

			size_t prevPt = (i > 0) ? i - 1 : bApmCount - 1;
			size_t nextPt = (i < bApmCount - 1) ? i + 1 : 0;

			wf::Vec2 prev = obj1.points[prevPt].position;
			wf::Vec2 next = obj1.points[nextPt].position;

			wf::Vec2 fromPrev = pt - prev;
			wf::Vec2 toNext = next - pt;

			wf::Vec2 v = fromPrev + toNext;
			wf::Vec2 ptNorm = glm::vec2(-v.y, v.x);

			float closestAway = 100000.0f;
			float closestSame = 100000.0f;

			infoAway.clear();
			infoAway.obj1 = &obj1;
			infoAway.obj1Point = i;
			infoAway.obj2 = &obj2;

			infoSame.clear();
			infoSame.obj1 = &obj1;
			infoSame.obj1Point = i;
			infoSame.obj2 = &obj2;

			bool found = false;
			size_t b1 = 0;
			size_t b2 = 1;

			for (size_t j = 0; j < bBpmCount; j++) {
				wf::Vec2 hitPt;
				wf::Vec2 norm;
				float edgeD;

				b1 = j;

				if (j < bBpmCount - 1) {
					b2 = j + 1;
				}
				else {
					b2 = 0;
				}

				EdgeCol ec = getClosestPointOnEdgeSquared(pt, obj2.edges[j]);
				float dist = ec.dist;

				hitPt = ec.hitPoint;
				norm = ec.normal;
				edgeD = ec.edgeD;

				float dot = glm::dot(ptNorm, norm);

				if (dot <= 0.f) {
					if (dist < closestAway) {
						closestAway = dist;
						infoAway.obj2PointA = b1;
						infoAway.obj2PointB = b2;
						infoAway.edgeD = edgeD;
						infoAway.hitPoint = hitPt;
						infoAway.normal = norm;
						infoAway.penetrationSq = dist;
						found = true;
					}
				}
				else {
					if (dist < closestSame) {
						closestSame = dist;
						infoSame.obj2PointA = b1;
						infoSame.obj2PointB = b2;
						infoSame.edgeD = edgeD;
						infoSame.hitPoint = hitPt;
						infoSame.normal = norm;
						infoSame.penetrationSq = dist;
					}
				}
			}

			if (found && (closestAway > m_penetrationThreshold) && (closestSame < closestAway)) {
				obj1.points[infoSame.obj1Point].insideAnother = true;
				m_collisions.push_back(infoSame);
				hasCollisions = true;
			}
			else {
				obj1.points[infoAway.obj1Point].insideAnother = true;
				m_collisions.push_back(infoAway);
				hasCollisions = true;
			}
		}

		return hasCollisions;
	}

	EdgeCol Collider::getClosestPointOnEdgeSquared(const wf::Vec2& pt, const Component::Edge& edge)
	{
		EdgeCol ret;

		wf::Vec2 toP = pt - wf::Vec2(edge.p1);

		wf::Vec2 n = perp(edge.dir);
		float x = glm::dot(toP, wf::Vec2(edge.dir));

		if (x <= 0.f) {
			ret.dist = glm::length2(pt - wf::Vec2(edge.p1));
			ret.hitPoint = edge.p1;
			ret.edgeD = 0.f;
			ret.normal = n;
		}
		else if (x >= edge.length) {
			ret.dist = glm::length2(pt - wf::Vec2(edge.p2));
			ret.hitPoint = edge.p2;
			ret.edgeD = 1.f;
			ret.normal = n;
		}
		else {
			ret.dist = cross2(toP, wf::Vec2(edge.dir));
			ret.dist *= ret.dist;
			ret.hitPoint = edge.p1 + (edge.dir * x);
			ret.edgeD = x / edge.length;
			ret.normal = n;
		}

		return ret;
	}

	bool Collider::checkCollisionPoint(const wf::Vec2 point, const std::vector<Component::PointMass>& points)
	{
		bool inside = false;

		if (points.size() > 2) {
			for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
				if (
					(points[i].position.y > point.y) != (points[j].position.y > point.y) &&
					(point.x < (points[j].position.x - points[i].position.x) * (point.y - points[i].position.y) / (points[j].position.y - points[i].position.y) + points[i].position.x)
					) {
					inside = !inside;
				}
			}
		}
		return inside;
	}

	float Collider::cross2(const wf::Vec2& vec, const wf::Vec2& other)
	{
		return (vec.x * other.y) - (vec.y * other.x);
	}

	wf::Vec2 Collider::perp(const wf::Vec2& vec)
	{
		return { -vec.y, vec.x };
	}
}