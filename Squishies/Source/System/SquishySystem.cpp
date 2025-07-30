#include "SquishySystem.h"

#include "Component/Squishy.h"
#include "Utils/Misc.h"

#include <vector>

namespace Squishies
{
	bool SquishySystem::init()
	{
		entityManager->onCreate<Component::Squishy>([&](wf::Entity entity) {
			createSquishy(entity);
			});

		m_worldBounds.extend({ -10.f, -10.f, -10.f });
		m_worldBounds.extend({ 10.f, 10.f, 10.f });

		return true;
	}

	void SquishySystem::update(float dt)
	{
		entityManager->each<Component::Squishy, wf::component::Geometry>(
			[&](Component::Squishy& squishy, wf::component::Geometry& geometry) {

				auto& verts = geometry.mesh->vertices;

				// update our actual mesh verts from our points
				for (size_t i = 0; i < verts.size(); i++) {
					if (verts[i].position != squishy.points[i].position) {
						geometry.mesh->needsUpdate = true;
						verts[i].position = squishy.points[i].position;
					}
				}
			});
	}

	void SquishySystem::fixedUpdate(float dt)
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
	//
	// @todo still some bits outstanding
	void SquishySystem::createSquishy(wf::Entity entity)
	{
		// set up the pointmasses for all of the vertices.
		// we can probably use the indices too for joints, though might be a little sloppy
		auto& softbody = entity.getComponent<Component::Squishy>();
		auto& mesh = entity.getComponent<wf::component::Geometry>().mesh;
		auto& transform = entity.getComponent<wf::component::Transform>();

		softbody.points.resize(mesh->vertices.size());

		// apply the transform then reset it. @todo rot/scale
		softbody.derivedPosition = transform.position;
		softbody.derivedRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		transform.position = {};
		transform.rotation = {};
		transform.scale = { 1.f, 1.f, 1.f };

		// gather up the original points and the derived global shape
		for (size_t i = 0; i < mesh->vertices.size(); i++) {
			softbody.points[i].originalPosition = mesh->vertices[i].position;

			auto newPos = softbody.points[i].originalPosition + softbody.derivedPosition;
			softbody.points[i].position = newPos;
			softbody.points[i].globalPosition = newPos;
		}

		// next build out the joints
		// firstly we'll just use the triangles (indices) of the mesh.
		std::unordered_set<uint64_t> edgeSet;

		auto hashEdge = [](size_t a, size_t b) {
			if (a > b) std::swap(a, b);
			return (uint64_t(a) << 32) | b;
			};

		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			size_t i0 = mesh->indices[i + 0];
			size_t i1 = mesh->indices[i + 1];
			size_t i2 = mesh->indices[i + 2];

			std::array<std::pair<size_t, size_t>, 3> edges = {
				std::pair{i0, i1}, std::pair{i1, i2}, std::pair{i2, i0}
			};

			for (auto& [a, b] : edges) {
				uint64_t key = hashEdge(a, b);
				if (edgeSet.insert(key).second) {
					auto& pa = mesh->vertices[a].position;
					auto& pb = mesh->vertices[b].position;
					float dist = glm::distance(pa, pb);
					softbody.joints.emplace_back(a, b, dist);
					printf("%zu %zu %.2f\n", a, b, dist);
				}
			}
		}

		// next we'll add some extra support around the edges (i.e. skip vertex 0, the center point)
		size_t ringStart = 1;
		size_t ringCount = mesh->vertices.size() - 1;

		for (size_t i = 0; i < ringCount; ++i)
		{
			size_t a = ringStart + i;
			size_t b = ringStart + ((i + 2) % ringCount); // skip one vertex, wrap around

			uint64_t key = hashEdge(a, b);
			if (edgeSet.insert(key).second)
			{
				auto& pa = mesh->vertices[a].position;
				auto& pb = mesh->vertices[b].position;
				float dist = glm::distance(pa, pb);
				softbody.joints.emplace_back(a, b, dist);
			}
		}

		softbody.updateDerivedData();
		softbody.updateEdges();
		softbody.updateBoundingBox();


		// @todo maybe we need to keep tabs on the inner points. if there are points where basically 
	}

	// 1. PREP: foreach body
	//		1. update shape meta for shape matching
	//		2. accumulate external forces - gravity, etc
	//		3. accumulate internal forces - spring joints, etc
	//
	// @todo check shape meta stuff for 3D
	void SquishySystem::prepareAndAccumulateForces()
	{
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

				// update details about perceived position/rotation/velocity of the overall body
				squishy.updateDerivedData();
				squishy.updateGlobalShape();

				// apply gravity
				for (auto& pt : squishy.points) {
					if (pt.fixed) continue;

					pt.force += wf::Vec3(0.f, m_gravity, 0.f) * pt.mass;
				}

				// internal forces - springs, shape matching, etc.
				// first the joints
				for (auto& [p1, p2, rest] : squishy.joints) {
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
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

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
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

				if (!m_worldBounds.isValid) return;

				auto& pms = squishy.points;

				wf::Vec3 damping = { .9f, .8f, .9f };

				for (size_t i = 0; i < pms.size(); i++) {
					// horizontals
					if (pms[i].position.x < m_worldBounds.min.x) {
						pms[i].position.x = m_worldBounds.min.x;
						pms[i].velocity.x = -pms[i].velocity.x * damping.x;
					}
					else if (pms[i].position.x > m_worldBounds.max.x) {
						pms[i].position.x = m_worldBounds.max.x;
						pms[i].velocity.x = -pms[i].velocity.x * damping.x;
					}

					if (pms[i].position.z < m_worldBounds.min.z) {
						pms[i].position.z = m_worldBounds.min.z;
						pms[i].velocity.z = -pms[i].velocity.z * damping.x;
					}
					else if (pms[i].position.z > m_worldBounds.max.z) {
						pms[i].position.z = m_worldBounds.max.z;
						pms[i].velocity.z = -pms[i].velocity.z * damping.x;
					}

					// vertical
					if (pms[i].position.y < m_worldBounds.min.y) {
						pms[i].position.y = m_worldBounds.min.y;
						pms[i].velocity.y = -pms[i].velocity.y * damping.y;
						pms[i].velocity.x *= damping.x;
						pms[i].velocity.z *= damping.z;
					}
					else if (pms[i].position.y > m_worldBounds.max.y) {
						pms[i].position.y = m_worldBounds.max.y;
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
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

				squishy.updateBoundingBox();
				squishy.updateEdges();
				squishy.updateBitfields(m_worldBounds, m_spatialGridSize);

				// clear our point details ready for collision detection
				for (auto& pt : squishy.points) {
					pt.insideAnother = false;
				}
			});
	}

	// 5. COLLISIONS: for each body and each other body
	//		- check collision and add to list
	//		- process all collisions when we've checked the lot
	//
	// @todo collision response
	void SquishySystem::handleCollisions()
	{
		auto view = entityManager->find<Component::Squishy>();

		// make sure we're starting fresh
		m_collider.reset();

		// loop the objects and see what's colliding
		view.each(
			[&](wf::EntityID id, Component::Squishy& squishy) {
				view.each(
					[&](wf::EntityID id2, Component::Squishy& squishy2) {

						if (id == id2) return;

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
	// @todo, collision box, grounded checks
	void SquishySystem::postUpdates()
	{
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

				// @todo reset collision box

				// dampen velocities
				for (auto& pt : squishy.points) {
					pt.velocity *= .999f;

					// @todo expand collision box
				}

				// @todo grounded check
			});
	}
}
