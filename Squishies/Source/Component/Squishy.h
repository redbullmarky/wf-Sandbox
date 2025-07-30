#pragma once
#include "Engine.h"

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
		wf::Vec3 velocity{};										// current velocity for the point
		wf::Vec3 force{};											// all forces (external & internal) applied
		float mass{ 1.f };											// point mass
		bool fixed{ false };										// fixed point; cannot move

		wf::Vec3 lastPosition{};									// keeping tabs on last position for stability
		wf::Vec3 originalPosition{};								// the original vertex position for shape matching
	};

	/**
	 * @brief Main squishy body.
	 *
	 * Whilst everything is stored implying 3D, generally we're working in 2D but leaving the door open...
	 */
	struct Squishy
	{
		std::vector<PointMass> points;								// all of our point masses
		std::vector<std::tuple<size_t, size_t, float>> joints;		// joints between the points to hold shape

		bool kinematic{ false };
		bool shapeMatching{ true };									// whether shape matching is enabled

		float jointK{ 500.f };										// spring strength and damping for joints
		float jointDamping{ 10.f };

		float shapeMatchK{ 50.f };									// spring strength and damping for shape matching
		float shapeMatchDamping{ 5.f };

		wf::Vec3 derivedPosition{};									// calculated position of the body as a whole
		wf::Quat derivedRotation{};									// calcualted rotation of the body
		wf::Vec3 derivedVelocity{};									// calculated velocity of the body
		wf::BoundingBox boundingBox{};								// cached bounding box from the mesh

		wf::EntityID debugEntity;									// shadow entity for debugging

		/**
		 * @brief Updates the percieved position, rotation and velocity based on how the points have moved
		 */
		void updateDerivedData(const wf::Mesh* mesh);
	};
}
