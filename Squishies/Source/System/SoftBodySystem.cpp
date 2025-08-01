#include "SoftBodySystem.h"

#include "Component/Character.h"
#include "Component/Collider.h"
#include "Component/SoftBody.h"
#include "Config.h"

#include <vector>

namespace Squishies
{
	SoftBodySystem::SoftBodySystem(wf::Scene* scene)
		:ISystem(scene), m_collider(scene->getEventDispatcher())
	{
	}

	bool SoftBodySystem::init()
	{
		entityManager->onCreate<Component::SoftBody>([&](wf::Entity entity) {
			createSquishy(entity);
			});

		return true;
	}

	void SoftBodySystem::update(float dt)
	{
		entityManager->each<Component::SoftBody, wf::component::Geometry>(
			[&](wf::EntityID id, Component::SoftBody& softbody, wf::component::Geometry& geometry) {

				entityManager->get(id).getComponent<wf::component::Material>().diffuse.colour = softbody.colour;

				auto& verts = geometry.mesh->vertices;
				verts[0].position = softbody.derivedPosition;

				// update our actual mesh verts from our points
				for (size_t i = 1; i < verts.size(); i++) {
					if (verts[i].position != softbody.points[i - 1].position) {
						geometry.mesh->needsUpdate = true;
						verts[i].position = softbody.points[i - 1].position;

					}
					//wf::Debug::filledCircle(verts[i].position, 4.f, wf::WHITE);
				}
			});
	}

	void SoftBodySystem::fixedUpdate(float dt)
	{
		prepareAndAccumulateForces();
		integrate(dt);
		hardConstraints();
		metaUpdates();
		handleCollisions();
		postUpdates();
	}

	// 0. BUILD
	//		1. foreach point, keep an original, update the global shape and reset transforms
	//		2. point the mesh
	//		3. build the joints
	void SoftBodySystem::createSquishy(wf::Entity entity)
	{
		// grab the body we'll be building from
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

		softbody.updateAll();
	}

	// 1. PREP: foreach body
	//		1. update shape meta for shape matching
	//		2. accumulate external forces - gravity, etc
	//		3. accumulate internal forces - spring joints, etc
	//
	// @todo check shape meta stuff for 3D
	void SoftBodySystem::prepareAndAccumulateForces()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& softbody) {

				if (softbody.fixed) return;

				// update details about perceived position/rotation/velocity of the overall body
				softbody.updateDerivedData();
				softbody.updateGlobalShape();

				// apply gravity
				for (auto& pt : softbody.points) {
					if (pt.fixed) continue;
					pt.force += wf::Vec3(0.f, Config::get().gravity, 0.f) * pt.mass;
				}

				// internal forces - springs, shape matching, etc.
				// first the joints
				for (auto& [p1, p2, rest] : softbody.shape.getJoints()) {
					if (softbody.points[p1].fixed && softbody.points[p2].fixed) {
						continue;
					}
					wf::Vec3 force = wf::getSpringForce(
						softbody.points[p1].position,
						softbody.points[p1].velocity,
						softbody.points[p2].position,
						softbody.points[p2].velocity,
						softbody.jointK,
						softbody.jointDamping,
						rest
					);

					if (!softbody.points[p1].fixed) softbody.points[p1].force += force;
					if (!softbody.points[p2].fixed) softbody.points[p2].force -= force;
				}

				if (softbody.shapeMatching) {
					for (size_t i = 0; i < softbody.points.size(); i++) {
						wf::Vec3 shapeMatchForce{};

						if (!softbody.kinematic) {
							shapeMatchForce = wf::getSpringForce(
								softbody.points[i].position,
								softbody.points[i].velocity,
								softbody.points[i].globalPosition,
								softbody.points[i].velocity,
								softbody.shapeMatchK,
								softbody.shapeMatchDamping,
								0.f
							);
						}
						else {
							shapeMatchForce = wf::getSpringForce(
								softbody.points[i].position,
								softbody.points[i].velocity,
								softbody.points[i].globalPosition,
								wf::Vec3{},
								softbody.shapeMatchK,
								softbody.shapeMatchDamping,
								0.f
							);
						}

						softbody.points[i].force += shapeMatchForce;
					}
				}
			});
	}

	// 2. INTEGRATE: foreach point on each body
	//		1. velocity += (force / mass) * dt;
	//		2. lastPosition = position;
	//		3. position += velocity * dt;
	//		4. force = { 0.f };
	void SoftBodySystem::integrate(float dt)
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& softbody) {

				if (softbody.fixed) return;

				for (size_t i = 0; i < softbody.points.size(); i++) {
					// avoid pointless division by doing pointless code.
					auto fm = softbody.points[i].mass > 1.f ? (softbody.points[i].force / softbody.points[i].mass) : softbody.points[i].force;
					softbody.points[i].velocity += fm * dt;

					softbody.points[i].lastPosition = softbody.points[i].position;
					softbody.points[i].position += softbody.points[i].velocity * dt;

					softbody.points[i].force = {};
				}
			});
	}

	// 3. HARD CONSTRAINTS: foreach point on each body
	//		constrain (primitive bounce) using these dampings:
	//			float dampingX = 0.9f;
	//			float dampingY = 0.8f;
	//			float dampingZ = 0.9f;
	void SoftBodySystem::hardConstraints()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& softbody) {

				if (softbody.fixed) return;

				auto worldBounds = Config::get().worldBounds;

				if (!worldBounds.isValid) return;

				auto& pms = softbody.points;

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
	void SoftBodySystem::metaUpdates()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& softbody) {

				if (softbody.fixed) return;

				softbody.updateAll();

				// clear our point details ready for collision detection
				for (auto& pt : softbody.points) {
					pt.insideAnother = false;
				}
			});
	}

	// 5. COLLISIONS: for each body and each other body
	//		- check collision and add to list
	//		- process all collisions when we've checked the lot
	void SoftBodySystem::handleCollisions()
	{
		auto view = entityManager->find<Component::SoftBody, Component::Collider>();

		// make sure we're starting fresh
		m_collider.reset();

		// loop the objects and see what's colliding
		view.each(
			[&](wf::EntityID id, Component::SoftBody& softbody, Component::Collider& collider) {
				view.each(
					[&](wf::EntityID id2, Component::SoftBody& softbody2, Component::Collider& collider2) {

						// no point if they're the same object or both fixed
						if (id == id2 || (softbody.fixed && softbody2.fixed)) return;

						// group vs mask says no
						if (!((collider.collisionMask & collider2.collisionGroup) && (collider2.collisionMask & collider.collisionGroup))) return;

						// now we can check
						softbody.colliding = m_collider.check(softbody, softbody2);
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
	void SoftBodySystem::postUpdates()
	{
		entityManager->each<Component::SoftBody>(
			[&](Component::SoftBody& softbody) {

				if (softbody.fixed) return;

				softbody.collisionBox.reset();

				// dampen velocities and expand our collision box with all points in collision, if any
				for (auto& pt : softbody.points) {
					pt.velocity *= .999f;

					if (pt.insideAnother) {
						softbody.collisionBox.extend(pt.position);
					}

					//wf::Debug::filledCircle(pt.position, 5.f, pt.insideAnother ? wf::RED : wf::YELLOW);
				}

				// @todo grounded check
			});
	}
}
