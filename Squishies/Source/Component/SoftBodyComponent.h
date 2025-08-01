#pragma once
#include "Engine.h"
#include "Poly/Squishy.h"

#include <bitset>

// @todo we need to play with the values a bit. on the old squishies game, we had:
//
// void AutoJoints(float k = 1000.f, float d = 15.f);
// void AddJoint(size_t p1, size_t p2, float k = 500.f, float damping = 10.f, float restLen = 0.f);
// 
// and within, for shape matching: float shapeSpringK{ 150.f };  float shapeSpringD{ 5.f };
// 
// it looks like *maybe* the AddJoint was used for the basic edge stuff. autojoints seems like it was only used in the loader in the event
// there were no joints set up.
//

namespace Squishies::Component
{
	struct PointMass
	{
		wf::Vec3 position{};										// point position
		wf::Vec3 velocity{};										// current velocity for the point
		wf::Vec3 force{};											// all forces (external & internal) applied
		float mass{ 1.f };											// point mass
		bool fixed{ false };										// fixed point; cannot move

		wf::Vec3 lastPosition{};									// keeping tabs on last position for stability
		wf::Vec3 globalPosition{};									// after transforming the original using derived vals

		bool insideAnother{ false };								// collision detection picked up that this point is inside another object
	};

	struct Edge
	{
		wf::Vec3 dir{};
		float length{};
		wf::Vec3 p1{};
		wf::Vec3 p2{};

		Edge() = default;
		Edge(wf::Vec3 p1, wf::Vec3 p2)
		{
			update(p1, p2);
		}

		void update(wf::Vec3 newP1, wf::Vec3 newP2)
		{
			p1 = newP1;
			p2 = newP2;

			wf::Vec3 e = p1 - p2; // @todo winding order
			length = glm::length(e);
			dir = glm::normalize(e);
		}

		bool operator==(const Edge& rhs) const { return p1 == rhs.p1 && p2 == rhs.p2 && dir == rhs.dir; }
		bool operator!=(const Edge& rhs) const { return p1 != rhs.p1 || p2 != rhs.p2 || dir != rhs.dir; }
	};

	/**
	 * @brief Main squishy body.
	 *
	 * Whilst everything is stored implying 3D, generally we're working in 2D but leaving the door open...
	 */
	struct SoftBody
	{
		Squishy shape;												// shape used to create the softbody along with joint data
		std::vector<PointMass> points;								// all of our point masses
		wf::Colour colour{ wf::WHITE };								// general colour; not used at the mo for anything more than debugging. @todo

		bool fixed{ false };										// if the body is entirely static.
		bool kinematic{ false };
		bool shapeMatching{ true };									// whether shape matching is enabled

		float jointK{ 300.f };										// spring strength and damping for joints
		float jointDamping{ 10.f };

		float shapeMatchK{ 150.f };									// spring strength and damping for shape matching
		float shapeMatchDamping{ 5.f };

		wf::Vec3 originalPosition{};								// original position, for resets
		wf::Quat originalRotation{};								// original rotation, for resets

		wf::Vec3 derivedPosition{};									// calculated position of the body as a whole
		wf::Quat derivedRotation{};									// calcualted rotation of the body
		wf::Vec3 derivedVelocity{};									// calculated velocity of the body

		bool colliding{ false };									// whether we're colliding with another
		wf::BoundingBox collisionBox{};								// bounding box containing all points currently colliding

		wf::Bitfields bitFields;									// simple space partitioning, for collisons
		wf::BoundingBox boundingBox{};								// cached bounding box from the mesh
		std::vector<Edge> edges;									// edge data

		/**
		 * @brief Update all metadata in one go
		 */
		void updateAll();

		/**
		 * @brief Updates the percieved position, rotation and velocity based on how the points have moved
		 */
		void updateDerivedData();

		/**
		 * @brief Regenerate the bounding box data
		 */
		void updateBoundingBox();

		/**
		 * @brief Update our global (world position) derived shape
		 */
		void updateGlobalShape();

		/**
		 * @brief Update bitmask based on our position for collision filtering
		 */
		void updateBitfields();

		/**
		 * @brief Update all of the edge data in prep for collision detection
		 */
		void updateEdges();

		/**
		 * @brief Set the body as immovable
		 * @param fixed
		 */
		void setFixed(bool fixed = true);

		/**
		 * @brief Create softbody from a squishy instance
		 */
		SoftBody(const Squishy& squishy);

	private:
		SoftBody() = default;
	};
}
