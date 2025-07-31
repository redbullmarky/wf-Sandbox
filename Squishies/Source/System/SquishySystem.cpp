#include "SquishySystem.h"

#include "Component/SoftBody.h"
#include "Config.h"

#include <vector>

namespace Squishies
{
	bool SquishySystem::init()
	{
		entityManager->onCreate<Component::SoftBody>([&](wf::Entity entity) {
			createSquishy(entity);
			});

		return true;
	}

	void SquishySystem::update(float dt)
	{
		entityManager->each<Component::SoftBody, wf::component::Geometry>(
			[&](Component::SoftBody& squishy, wf::component::Geometry& geometry) {

				auto& verts = geometry.mesh->vertices;

				verts[0].position = squishy.derivedPosition;

				// update our actual mesh verts from our points
				for (size_t i = 1; i < verts.size(); i++) {
					if (verts[i].position != squishy.points[i - 1].position) {
						geometry.mesh->needsUpdate = true;
						verts[i].position = squishy.points[i - 1].position;
					}
				}
			});
	}

	void SquishySystem::fixedUpdate(float dt)
	{
		prepareAndAccumulateForces();
		integrate(dt);
		hardConstraints();
		for (int i = 0; i < 10; i++) {
			metaUpdates();
			handleCollisions();
			postUpdates();
		}
	}

	// 0. BUILD
	//		1. foreach point, keep an original, update the global shape and reset transforms
	//		2. point the mesh
	//		3. build the joints
	void SquishySystem::createSquishy(wf::Entity entity)
	{
		auto& softbody = entity.getComponent<Component::SoftBody>();

		// create the dynamic geometry
		auto& points = softbody.shape.getPoints();
		auto& geometry = entity.addComponent<wf::component::Geometry>(softbody.shape.createMesh());
		geometry.mesh->isDynamic = true;
		entity.getComponent<wf::component::Material>().diffuse.colour = softbody.colour;

		// set up the pointmasses for all of the vertices.
		// we can probably use the indices too for joints, though might be a little sloppy
		auto& transform = entity.getComponent<wf::component::Transform>();
		auto& mesh = geometry.mesh;

		softbody.points.resize(points.size());

		// apply the transform then reset it. @todo rot/scale
		softbody.derivedPosition = softbody.originalPosition = transform.position;
		softbody.derivedRotation = softbody.originalRotation = glm::quat(glm::radians(transform.rotation));
		transform.position = {};
		transform.rotation = {};
		transform.scale = { 1.f, 1.f, 1.f };

		// gather up the original points and the derived global shape
		for (size_t i = 0; i < points.size(); i++) {
			// get our point position in worldspace
			auto newPos = wf::Vec3(softbody.shape.getPoint(i), 0.f) + softbody.derivedPosition;

			// store it in position and globalPosition
			softbody.points[i].position = newPos;
			softbody.points[i].globalPosition = newPos;
		}

		softbody.updateDerivedData();
		softbody.updateEdges();
		softbody.updateBoundingBox();
	}

	// 1. PREP: foreach body
	//		1. update shape meta for shape matching
	//		2. accumulate external forces - gravity, etc
	//		3. accumulate internal forces - spring joints, etc
	//
	// @todo check shape meta stuff for 3D
	void SquishySystem::prepareAndAccumulateForces()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& squishy) {

				if (squishy.fixed) return;

				// update details about perceived position/rotation/velocity of the overall body
				squishy.updateDerivedData();
				squishy.updateGlobalShape();

				// apply gravity
				for (auto& pt : squishy.points) {
					if (pt.fixed) continue;
					pt.force += wf::Vec3(0.f, Config::get().gravity, 0.f) * pt.mass;
				}

				// internal forces - springs, shape matching, etc.
				// first the joints
				for (auto& [p1, p2, rest] : squishy.shape.getJoints()) {
					if (squishy.points[p1].fixed && squishy.points[p2].fixed) {
						continue;
					}
					wf::Vec3 force = wf::getSpringForce(
						squishy.points[p1].position,
						squishy.points[p1].velocity,
						squishy.points[p2].position,
						squishy.points[p2].velocity,
						squishy.jointK,
						squishy.jointDamping,
						rest
					);

					if (!squishy.points[p1].fixed) squishy.points[p1].force += force;
					if (!squishy.points[p2].fixed) squishy.points[p2].force -= force;
				}

				if (squishy.shapeMatching) {
					for (size_t i = 0; i < squishy.points.size(); i++) {
						wf::Vec3 shapeMatchForce{};

						if (!squishy.kinematic) {
							shapeMatchForce = wf::getSpringForce(
								squishy.points[i].position,
								squishy.points[i].velocity,
								squishy.points[i].globalPosition,
								squishy.points[i].velocity,
								squishy.shapeMatchK,
								squishy.shapeMatchDamping,
								0.f
							);
						}
						else {
							shapeMatchForce = wf::getSpringForce(
								squishy.points[i].position,
								squishy.points[i].velocity,
								squishy.points[i].globalPosition,
								wf::Vec3{},
								squishy.shapeMatchK,
								squishy.shapeMatchDamping,
								0.f
							);
						}

						squishy.points[i].force += shapeMatchForce;
					}
				}
			});
	}

	// 2. INTEGRATE: foreach point on each body
	//		1. velocity += (force / mass) * dt;
	//		2. lastPosition = position;
	//		3. position += velocity * dt;
	//		4. force = { 0.f };
	void SquishySystem::integrate(float dt)
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& squishy) {

				if (squishy.fixed) return;

				for (size_t i = 0; i < squishy.points.size(); i++) {
					// avoid pointless division by doing pointless code.
					auto fm = squishy.points[i].mass > 1.f ? (squishy.points[i].force / squishy.points[i].mass) : squishy.points[i].force;
					squishy.points[i].velocity += fm * dt;

					squishy.points[i].lastPosition = squishy.points[i].position;
					squishy.points[i].position += squishy.points[i].velocity * dt;

					squishy.points[i].force = {};
				}
			});
	}

	// 3. HARD CONSTRAINTS: foreach point on each body
	//		constrain (primitive bounce) using these dampings:
	//			float dampingX = 0.9f;
	//			float dampingY = 0.8f;
	//			float dampingZ = 0.9f;
	void SquishySystem::hardConstraints()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& squishy) {

				if (squishy.fixed) return;

				auto worldBounds = Config::get().worldBounds;

				if (!worldBounds.isValid) return;

				auto& pms = squishy.points;

				wf::Vec3 damping = { .9f, .8f, .9f };

				for (size_t i = 0; i < pms.size(); i++) {
					// horizontals
					if (pms[i].position.x < worldBounds.min.x) {
						pms[i].position.x = worldBounds.min.x;
						pms[i].velocity.x = -pms[i].velocity.x * damping.x;
					}
					else if (pms[i].position.x > worldBounds.max.x) {
						pms[i].position.x = worldBounds.max.x;
						pms[i].velocity.x = -pms[i].velocity.x * damping.x;
					}

					if (pms[i].position.z < worldBounds.min.z) {
						pms[i].position.z = worldBounds.min.z;
						pms[i].velocity.z = -pms[i].velocity.z * damping.x;
					}
					else if (pms[i].position.z > worldBounds.max.z) {
						pms[i].position.z = worldBounds.max.z;
						pms[i].velocity.z = -pms[i].velocity.z * damping.x;
					}

					// vertical
					if (pms[i].position.y < worldBounds.min.y) {
						pms[i].position.y = worldBounds.min.y;
						pms[i].velocity.y = -pms[i].velocity.y * damping.y;
						pms[i].velocity.x *= damping.x;
						pms[i].velocity.z *= damping.z;
					}
					else if (pms[i].position.y > worldBounds.max.y) {
						pms[i].position.y = worldBounds.max.y;
						pms[i].velocity.y = -pms[i].velocity.y * damping.y;
						pms[i].velocity.x *= damping.x;
						pms[i].velocity.z *= damping.z;
					}
				}
			});
	}

	// 4. META UPDATES: foreach body
	//		1. update bounding box
	//		2. update edge data
	//		3. update bitmask (where in the world on the "grid" for collisions)
	void SquishySystem::metaUpdates()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& squishy) {

				if (squishy.fixed) return;

				squishy.updateBoundingBox();
				squishy.updateEdges();
				squishy.updateBitfields(Config::get().worldBounds, Config::get().spatialGridSize);

				// clear our point details ready for collision detection
				for (auto& pt : squishy.points) {
					pt.insideAnother = false;
				}
			});
	}

	// 5. COLLISIONS: for each body and each other body
	//		- check collision and add to list
	//		- process all collisions when we've checked the lot
	void SquishySystem::handleCollisions()
	{
		auto view = entityManager->find<Component::SoftBody>();

		// make sure we're starting fresh
		m_collider.reset();

		// loop the objects and see what's colliding
		view.each(
			[&](wf::EntityID id, Component::SoftBody& squishy) {
				view.each(
					[&](wf::EntityID id2, Component::SoftBody& squishy2) {

						if (id == id2 || squishy.fixed && squishy2.fixed) return;

						squishy.colliding = m_collider.check(squishy, squishy2);
					});
			});

		// now handle any collisions we found
		m_collider.respond();
	}

	// 6. POST-UPDATES: foreach body
	//		1. reset collision box
	//		2. foreach point
	//			1. damp velocity by 0.999f
	//			2. expand the collision box if the point is inside of another (gathered during collision check)
	//		3. determine if the body is grounded
	//
	// @todo grounded checks
	void SquishySystem::postUpdates()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& squishy) {

				if (squishy.fixed) return;

				squishy.collisionBox.reset();

				// dampen velocities and expand our collision box with all points in collision, if any
				for (auto& pt : squishy.points) {
					pt.velocity *= .999f;

					if (pt.insideAnother) {
						squishy.collisionBox.extend(pt.position);
					}

					//wf::Debug::filledCircle(pt.position, 5.f, pt.insideAnother ? wf::RED : wf::YELLOW);
				}

				// @todo grounded check
			});
	}
}
