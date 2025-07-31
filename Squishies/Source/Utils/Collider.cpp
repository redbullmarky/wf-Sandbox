#include "Collider.h"
#include "Engine.h"

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
	bool Collider::check(Component::SoftBody& obj1, Component::SoftBody& obj2)
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

			size_t prevPt = (i > 0) ? 0 : bApmCount - 1;
			size_t nextPt = (i + 1) % bApmCount;

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
				b2 = (j + 1) % bBpmCount;

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

	void Collider::respond()
	{
		int penCount = 0;

		for (size_t i = 0; i < m_collisions.size(); i++) {
			const Collision& info = m_collisions[i];

			// @todo this was quite useful but maybe a better way now we know what we're doing a bit more...
			/*if (NotifyCollision(info)) {
				continue;
			}*/

			auto& pointA = info.obj1->points[info.obj1Point];
			auto& pointB1 = info.obj2->points[info.obj2PointA];
			auto& pointB2 = info.obj2->points[info.obj2PointB];

			wf::Debug::line(pointA.position, pointA.position + wf::Vec3(info.normal, 0.f) * 20.f, 2.f, info.obj1->colour);


			// fixme doing this also for kinematics, but this might be better to use derivedVelocity...if we calc it for kinematic objects.
			wf::Vec2 bVel = ((pointB1.fixed ? wf::Vec2{} : wf::Vec2(pointB1.velocity)) + (pointB2.fixed ? wf::Vec2{} : pointB2.velocity)) * .5f;
			wf::Vec2 relVel = wf::Vec2(pointA.velocity) - bVel;
			float relDot = glm::dot(relVel, info.normal);

			// nothing to do if the points are moving away from eachother already @todo assess this
			/*if (relDot >= 0.f) {
				continue;
			}*/

			if (info.penetrationSq > (m_penetrationThreshold * m_penetrationThreshold)) {
				penCount++;
				continue;
			}

			float penetration = sqrt(info.penetrationSq);

			float b1inf = 1.f - info.edgeD;
			float b2inf = info.edgeD;

			float b2MassSum = (pointB1.fixed || pointB2.fixed) ? std::numeric_limits<float>::infinity() : (pointB1.mass + pointB2.mass);
			float massSum = pointA.mass + b2MassSum;

			float Amove = 0.f;
			float Bmove = 0.f;

			if (pointA.fixed) {
				Bmove = penetration + 0.001f;
			}
			else if (std::isinf(b2MassSum)) {
				Amove = penetration + 0.001f;
			}
			else {
				Amove = (penetration * (b2MassSum / massSum));
				Bmove = (penetration * (pointA.mass / massSum));
			}

			float pointB1move = Bmove * b1inf;
			float pointB2move = Bmove * b2inf;

			float AinvMass = pointA.fixed ? 0.f : 1.f / pointA.mass;
			float BinvMass = std::isinf(b2MassSum) ? 0.f : 1.f / b2MassSum;

			float jDenom = AinvMass + BinvMass;
			wf::Vec2 numV = relVel * (1.f + m_elasticity);

			float jNumerator = glm::dot(numV, info.normal);
			jNumerator = -jNumerator;

			float j = jNumerator / jDenom;

			if (!pointA.fixed) {
				pointA.position += wf::Vec3(info.normal * Amove, 0.f);
			}

			if (!pointB1.fixed) {
				pointB1.position -= wf::Vec3(info.normal * pointB1move, 0.f);
			}

			if (!pointB2.fixed) {
				pointB2.position -= wf::Vec3(info.normal * pointB2move, 0.f);
			}

			wf::Vec2 tangent = perp(info.normal);
			float fNumerator = glm::dot(relVel, tangent) * m_friction;
			float f = fNumerator / jDenom;

			if (relDot < 0.0001f) {
				if (!pointA.fixed) {
					pointA.velocity += wf::Vec3((info.normal * (j / pointA.mass)) - (tangent * (f / pointA.mass)), 0.f);
				}

				if (!pointB1.fixed) {
					pointB1.velocity -= wf::Vec3((info.normal * (j / b2MassSum) * b1inf) - (tangent * (f / b2MassSum) * b1inf), 0.f);
				}

				if (!pointB2.fixed) {
					pointB2.velocity -= wf::Vec3((info.normal * (j / b2MassSum) * b2inf) - (tangent * (f / b2MassSum) * b2inf), 0.f);
				}
			}
		}
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