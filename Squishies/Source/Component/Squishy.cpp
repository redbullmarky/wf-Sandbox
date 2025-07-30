#include "Squishy.h"
#include "Engine.h"

#include <glm/glm.hpp>

namespace Squishies::Component
{
	void Squishy::updateDerivedData()
	{
		// skip the center
		size_t pointCount = this->points.size() - 1;

		// first figure out the center and velocity
		const float invPCount = 1.f / pointCount;
		wf::Vec3 center{};
		wf::Vec3 velocity{};

		for (size_t i = 1; i < this->points.size(); i++) {
			center += this->points[i].position;
			velocity += this->points[i].velocity;
		}

		this->derivedPosition = center * invPCount;
		this->derivedVelocity = velocity * invPCount;

		// now find the average angle
		// Original and current average normals
		// Project all positions to X/Y plane and calculate average angle offset
		float angleSum = 0.f;

		for (size_t i = 1; i < this->points.size(); i++) {
			glm::vec2 base2 = glm::vec2(this->points[i].originalPosition);
			glm::vec2 curr2 = glm::vec2(this->points[i].position - this->derivedPosition);

			float baseAngle = atan2(base2.y, base2.x);
			float currAngle = atan2(curr2.y, curr2.x);

			float delta = currAngle - baseAngle;
			if (delta > glm::pi<float>()) delta -= glm::two_pi<float>();
			if (delta < -glm::pi<float>()) delta += glm::two_pi<float>();
			angleSum += delta;
		}

		float avgAngle = angleSum / pointCount;
		this->derivedRotation = glm::angleAxis(avgAngle, glm::vec3(0, 0, 1));
	}

	void Squishy::updateBoundingBox()
	{
		boundingBox.reset();

		for (auto& v : this->points) {
			boundingBox.extend(v.position);
			boundingBox.extend(v.position + wf::Vec3{ 0.f, 0.f, .1f }); // @todomay revisit, but we're nudging a little bit for the z
		}
	}

	void Squishy::updateGlobalShape()
	{
		for (auto& pt : this->points) {
			pt.globalPosition = pt.originalPosition + this->derivedPosition;
			// @todo apply rotation
		}
	}

	void Squishy::updateBitfields(const wf::BoundingBox& worldBounds, float gridCellSize)
	{
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

	void Squishy::updateEdges()
	{
		if (this->edges.size() != this->points.size()) {
			this->edges.resize(this->points.size());
		}

		// skip the first vertex as it's the center
		for (size_t i = 1; i < this->points.size(); i++) {
			size_t j = (i == this->points.size() - 1) ? 1 : i + 1;

			this->edges[i - 1].update(this->points[i].position, this->points[j].position);
		}
	}
}