#pragma once
#include "Engine.h"
#include "Component/SoftBodyComponent.h"

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

	struct CollisionData
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
		Collider(wf::EventDispatcher* eventDispatcher);
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
		void respond();

	private:
		EdgeCol getClosestPointOnEdgeSquared(const wf::Vec2& pt, const Component::Edge& edge);
		bool checkCollisionPoint(const wf::Vec2 point, const std::vector<Component::PointMass>& points);

	private:
		wf::EventDispatcher* m_eventDispatcher{ nullptr };

		std::vector<CollisionData> m_collisions;

		// config
		float m_penetrationThreshold{ .3f };
		float m_elasticity{ .8f };
		float m_friction{ .3f };
	};
}