#include "SoftBodyComponent.h"
#include "Engine.h"

#include "Config.h"

#include <glm/glm.hpp>

namespace Squishies::Component
{
	void SoftBody::updateAll()
	{
		updateDerivedData();
		updateEdges();
		updateBoundingBox();
		updateBitfields();
	}

	void SoftBody::updateDerivedData()
	{
		// first figure out the center and velocity
		const float invPCount = 1.f / this->points.size();
		wf::Vec3 center{};
		wf::Vec3 velocity{};

		for (size_t i = 0; i < this->points.size(); i++) {
			center += this->points[i].position;
			velocity += this->points[i].velocity;
		}

		this->derivedPosition = center * invPCount;
		this->derivedVelocity = velocity * invPCount;

		// now find the average angle
		// Original and current average normals
		// Project all positions to X/Y plane and calculate average angle offset
		float angleSum = 0.f;

		for (size_t i = 0; i < this->points.size(); i++) {
			glm::vec2 base2 = glm::vec2(this->shape.getPoint(i));
			glm::vec2 curr2 = glm::vec2(this->points[i].position - this->derivedPosition);

			float baseAngle = atan2(base2.y, base2.x);
			float currAngle = atan2(curr2.y, curr2.x);

			float delta = currAngle - baseAngle;
			if (delta > glm::pi<float>()) delta -= glm::two_pi<float>();
			if (delta < -glm::pi<float>()) delta += glm::two_pi<float>();
			angleSum += delta;
		}

		float avgAngle = angleSum / this->points.size();
		this->derivedRotation = glm::angleAxis(avgAngle, glm::vec3(0, 0, 1));
	}

	void SoftBody::updateBoundingBox()
	{
		boundingBox.reset();

		for (auto& v : this->points) {
			boundingBox.extend(v.position);
			boundingBox.extend(v.position + wf::Vec3{ 0.f, 0.f, .1f }); // @todo may revisit, but we're nudging a little bit for the z
		}
	}

	void SoftBody::updateGlobalShape()
	{
		for (size_t i = 0; i < this->points.size(); i++) {
			this->points[i].globalPosition = wf::Vec3(this->shape.getPoint(i), 0.f) + this->derivedPosition;
			// @todo apply rotation
		}
	}

	void SoftBody::updateBitfields()
	{
		auto worldBounds = Config::get().worldBounds;
		auto gridCellSize = Config::get().spatialGridSize;

		wf::Vec3 gridStep = worldBounds.size() / gridCellSize;

		int minX = (int)floor((boundingBox.min.x - worldBounds.min.x) / gridStep.x);
		int maxX = (int)floor((boundingBox.max.x - worldBounds.min.x) / gridStep.x);
		int minY = (int)floor((boundingBox.min.y - worldBounds.min.y) / gridStep.y);
		int maxY = (int)floor((boundingBox.max.y - worldBounds.min.y) / gridStep.y);
		int minZ = (int)floor((boundingBox.min.z - worldBounds.min.z) / gridStep.z);
		int maxZ = (int)floor((boundingBox.max.z - worldBounds.min.z) / gridStep.z);

		minX = glm::clamp(minX, 0, static_cast<int>(gridCellSize));
		minY = glm::clamp(minY, 0, static_cast<int>(gridCellSize));
		minZ = glm::clamp(minZ, 0, static_cast<int>(gridCellSize));
		maxX = glm::clamp(maxX, 0, static_cast<int>(gridCellSize));
		maxY = glm::clamp(maxY, 0, static_cast<int>(gridCellSize));
		maxZ = glm::clamp(maxZ, 0, static_cast<int>(gridCellSize));

		this->bitFields.clear();

		for (int i = minX; i <= maxX; i++) {
			this->bitFields.x.setOn(i);
		}
		for (int i = minY; i <= maxY; i++) {
			this->bitFields.y.setOn(i);
		}
		for (int i = minZ; i <= maxZ; i++) {
			this->bitFields.z.setOn(i);
		}
	}

	void SoftBody::updateEdges()
	{
		if (this->edges.size() != this->points.size()) {
			this->edges.resize(this->points.size());
		}

		for (size_t i = 0; i < this->points.size(); i++) {
			size_t j = (i + 1) % this->points.size();

			this->edges[i].update(this->points[i].position, this->points[j].position);

			// debug the edges
			/*wf::Vec3 center = (this->edges[i].p1 + this->edges[i].p2) * 0.5f;
			wf::Vec3 normal = glm::normalize(wf::Vec3{ -this->edges[i].dir.y, this->edges[i].dir.x, 0.f });
			wf::Debug::line(center, center + normal * .2f, 2.f, wf::WHITE);*/
		}
	}

	void SoftBody::setFixed(bool fixed)
	{
		this->fixed = fixed;
	}

	SoftBody::SoftBody(const Squishy& squishy) : shape(squishy), colour(squishy.colour)
	{
	}
}