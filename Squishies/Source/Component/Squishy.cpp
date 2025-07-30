#include "Squishy.h"
#include "Engine.h"

namespace Squishies::Component
{
	void Squishy::updateDerivedData()
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
			glm::vec2 base2 = glm::vec2(this->points[i].originalPosition);
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

	void Squishy::updateBoundingBox()
	{
		boundingBox.reset();

		for (auto& v : this->points) {
			boundingBox.extend(v.position);
		}
	}

	void Squishy::updateGlobalShape()
	{
		for (auto& pt : this->points) {
			pt.globalPosition = pt.originalPosition + this->derivedPosition;
			// @todo apply rotation
		}
	}
}