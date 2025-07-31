#pragma once
#include "Engine.h"
#include "Component/SoftBody.h"

#include <vector>

/**
 * @brief Collision detection and handling.
 *
 * Currently 2D only as ported from the previous game
 */
namespace Squishies
{
	struct EdgeCol
	{
		wf::Vec2 hitPoint{};
		wf::Vec2 normal{};
		float edgeD{};
		float dist{};
	};

	struct Collision
	{
		Component::SoftBody* obj1{ nullptr };
		size_t obj1Point{};

		Component::SoftBody* obj2{ nullptr };
		size_t obj2PointA{};
		size_t obj2PointB{};

		wf::Vec2 hitPoint{ 0.f };
		wf::Vec2 normal{ 0.f };
		float edgeD{ 0.f };
		float penetrationSq{ 0.f };

		void clear()
		{
			obj1 = obj2 = nullptr;
			obj1Point = obj2PointA = obj2PointB = -1;
			hitPoint = normal = {};
			edgeD = penetrationSq = 0.0f;
		}
	};

	class Collider
	{
	public:
		Collider() = default;
		~Collider() = default;

		/**
		 * @brief Configure the settings for the collider
		 * @param penetrationThreshold How far a point needs to pass into an object @todo confirm details
		 * @param elasticity How elastic the collisions are
		 * @param friction Friction between the bodies @todo probs want this per object
		 */
		void setup(float penetrationThreshold, float elasticity, float friction);

		/**
		 * @brief Clear out all previous data
		 */
		void reset();

		/**
		 * @brief Determine if we've had a collision and updates data about it
		 * @param obj1
		 * @param obj2
		 * @return
		 */
		bool check(Component::SoftBody& obj1, Component::SoftBody& obj2);

		/**
		 * @brief Process all of the collisions we detected
		 */
		void respond()
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

	private:
		EdgeCol getClosestPointOnEdgeSquared(const wf::Vec2& pt, const Component::Edge& edge);
		bool checkCollisionPoint(const wf::Vec2 point, const std::vector<Component::PointMass>& points);
		float cross2(const wf::Vec2& vec, const wf::Vec2& other);
		wf::Vec2 perp(const wf::Vec2& vec);

	private:
		std::vector<Collision> m_collisions;

		// config
		float m_penetrationThreshold{ .3f };
		float m_elasticity{ .8f };
		float m_friction{ .3f };
	};
}