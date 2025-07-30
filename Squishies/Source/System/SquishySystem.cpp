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

	void SquishySystem::setup()
	{
		entityManager->each<Component::Squishy, const wf::component::Geometry>(
			[&](Component::Squishy& squishy, const wf::component::Geometry& geometry) {
				auto obj = scene->createObject();
				obj.addComponent<wf::component::Geometry>(wf::mesh::createCircle(1.f, 15));
				auto& material = obj.addComponent<wf::component::Material>();
				material.wireframe = true;

				squishy.debugEntity = obj.handle;
			});
	}

	void SquishySystem::update(float dt)
	{
		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {
				auto debugEnt = entityManager->get(squishy.debugEntity);
				auto& transform = debugEnt.getComponent<wf::component::Transform>();
				transform.position = squishy.derivedPosition;
			});
	}

	void SquishySystem::render(float dt)
	{
		entityManager->each<Component::Squishy, const wf::component::Geometry>(
			[&](Component::Squishy& squishy, const wf::component::Geometry& geometry) {

			});
	}

	void SquishySystem::fixedUpdate(float dt)
	{
		prepareAndAccumulateForces();
		integrate(dt);
		hardConstraints();
		metaUpdates();
		collisionDetection();
		collisionResponse();
		postUpdates();
	}

	void SquishySystem::createSquishy(wf::Entity entity)
	{
		// set up the pointmasses for all of the vertices.
		// we can probably use the indices too for joints, though might be a little sloppy
		auto& softbody = entity.getComponent<Component::Squishy>();
		auto& mesh = entity.getComponent<wf::component::Geometry>().mesh;
		auto& transform = entity.getComponent<wf::component::Transform>();

		softbody.points.resize(mesh->vertices.size());

		// first up - apply the transform to the vertices so we're in world space.
		// along the way, we'll save the global shape
		auto transformMatrix = transform.getTransformMatrix();
		for (size_t i = 0; i < mesh->vertices.size(); i++) {
			glm::vec4 pos = glm::vec4(mesh->vertices[i].position, 1.f);
			mesh->vertices[i].position = glm::vec3(transformMatrix.matrix * pos);

			// keep the point
			softbody.points[i].originalPosition = mesh->vertices[i].position;
		}
		// now we've shifted all the points to world pos, we can reset the transform matrix
		transform.position = {};
		transform.rotation = {};
		transform.scale = { 1.f, 1.f, 1.f };

		// next build out the joints
		std::unordered_set<uint64_t> edgeSet;
		std::vector<std::tuple<size_t, size_t, float>> joints;

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

			for (auto [a, b] : edges) {
				uint64_t key = hashEdge(a, b);
				if (edgeSet.insert(key).second) {
					auto& pa = mesh->vertices[a].position;
					auto& pb = mesh->vertices[b].position;
					float dist = glm::distance(pa, pb);
					joints.emplace_back(a, b, dist);
				}
			}
		}

		// @todo there's perhaps some other stuff we need to pre-set here.
		softbody.updateDerivedData(mesh.get());

		// @todo maybe we need to keep tabs on the inner points. if there are points where basically 
	}

	void SquishySystem::prepareAndAccumulateForces()
	{
		// 1. PREP: foreach body
		//		1. update shape meta for shape matching
		//		2. accumulate external forces - gravity, etc
		//		3. accumulate internal forces - spring joints, etc

		entityManager->each<Component::Squishy, const wf::component::Geometry>(
			[&](Component::Squishy& squishy, const wf::component::Geometry& geometry) {

				auto& verts = geometry.mesh->vertices;

				// update details about perceived position/rotation/velocity of the overall body
				squishy.updateDerivedData(geometry.mesh.get());

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
						verts[p1].position,
						squishy.points[p1].velocity,
						verts[p2].position,
						squishy.points[p2].velocity,
						squishy.jointK,
						squishy.jointDamping,
						rest
					);

					if (!squishy.points[p1].fixed) squishy.points[p1].force += force;
					if (!squishy.points[p2].fixed) squishy.points[p2].force -= force;
				}

				if (squishy.shapeMatching) {

					std::vector<wf::Vec3> globalPoints;
					globalPoints.reserve(squishy.points.size());

					for (auto& pt : squishy.points) {
						globalPoints.push_back(pt.originalPosition);
					}

					globalPoints = transformPoints(globalPoints, squishy.derivedPosition, squishy.derivedRotation);

					for (size_t i = 0; i < verts.size(); i++) {
						wf::Vec3 shapeMatchForce{};

						if (!squishy.kinematic) {
							shapeMatchForce = wf::getSpringForce(
								verts[i].position,
								squishy.points[i].velocity,
								globalPoints[i],
								squishy.points[i].velocity,
								squishy.shapeMatchK,
								squishy.shapeMatchDamping,
								0.f
							);
						}
						else {
							shapeMatchForce = wf::getSpringForce(
								verts[i].position,
								squishy.points[i].velocity,
								globalPoints[i],
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

	void SquishySystem::integrate(float dt)
	{
		// 2. INTEGRATE: foreach point on each body
		//		1. velocity += (force / mass) * dt;
		//		2. lastPosition = position;
		//		3. position += velocity * dt;
		//		4. force = { 0.f };

		entityManager->each<Component::Squishy, wf::component::Geometry>(
			[&](Component::Squishy& squishy, wf::component::Geometry& geometry) {

				auto& verts = geometry.mesh->vertices;

				for (size_t i = 0; i < verts.size(); i++) {
					// avoid pointless division by doing pointless code.
					auto fm = squishy.points[i].mass > 1.f ? (squishy.points[i].force / squishy.points[i].mass) : squishy.points[i].force;
					squishy.points[i].velocity += fm * dt;

					squishy.points[i].lastPosition = verts[i].position;
					verts[i].position += squishy.points[i].velocity * dt;

					squishy.points[i].force = {};
				}
				geometry.mesh->needsUpdate = true;
			});
	}

	void SquishySystem::hardConstraints()
	{
		// 3. HARD CONSTRAINTS: foreach point on each body
		//		constrain (primitive bounce) using these dampings:
		//			float dampingX = 0.9f;
		//			float dampingY = 0.8f;

		entityManager->each<Component::Squishy, wf::component::Geometry>(
			[&](Component::Squishy& squishy, wf::component::Geometry& geometry) {

				if (!m_worldBounds.isValid) return;

				auto& verts = geometry.mesh->vertices;
				auto& pms = squishy.points;

				// @todo this is only in the X/Y dims for now
				float dampingX = 0.9f;
				float dampingY = 0.8f;

				for (size_t i = 0; i < verts.size(); i++) {
					if (verts[i].position.x < m_worldBounds.min.x) {
						verts[i].position.x = m_worldBounds.min.x;
						pms[i].velocity.x = -pms[i].velocity.x * dampingX;
					}
					else if (verts[i].position.x > m_worldBounds.max.x) {
						verts[i].position.x = m_worldBounds.max.x;
						pms[i].velocity.x = -pms[i].velocity.x * dampingX;
					}

					if (verts[i].position.y < m_worldBounds.min.y) {
						verts[i].position.y = m_worldBounds.min.y;
						pms[i].velocity.y = -pms[i].velocity.y * dampingY;
						pms[i].velocity.x *= dampingX;
					}
					else if (verts[i].position.y > m_worldBounds.max.y) {
						verts[i].position.y = m_worldBounds.max.y;
						pms[i].velocity.y = -pms[i].velocity.y * dampingY;
						pms[i].velocity.x *= dampingX;
					}
				}
			});
	}

	void SquishySystem::metaUpdates()
	{
		// 4. META UPDATES: foreach body
		//		1. update bounding box
		//		2. update edge data
		//		3. update bitmask (where in the world on the "grid" for collisions)

		entityManager->each<Component::Squishy, const wf::component::Geometry>(
			[&](Component::Squishy& squishy, const wf::component::Geometry& geometry) {

				squishy.boundingBox = geometry.mesh->getBoundingBox();

				// edge data


				// bitmask
			});
	}

	void SquishySystem::collisionDetection()
	{
		// 5. COLLISIONS: for each body and each other body
		//		check collision and add to list

	}

	void SquishySystem::collisionResponse()
	{
		// 6. COLLISON RESPONSE: for each collision
		//		resolve the collision

	}

	void SquishySystem::postUpdates()
	{
		// 7. POST-UPDATES: foreach body
		//		1. reset collision box
		//		2. foreach point
		//			1. damp velocity by 0.999f
		//			2. expand the collision box if the point is inside of another (gathered during collision check)
		//		3. determine if the body is grounded

		entityManager->each<Component::Squishy>(
			[&](Component::Squishy& squishy) {

			});
	}
}
