#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float RAD2DEG = 180.f / 3.14159265358979323846f; // ~ 57.2957795131
constexpr float DEG2RAD = 3.14159265358979323846f / 180.f; // ~ 0.0174532925
constexpr float PI = 3.14159265359f;

namespace wf
{
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;
	using Quat = glm::quat;

	struct Mat4
	{
		glm::mat4 matrix;

		Mat4& translate(const Vec3& pt);
		Mat4& scale(const Vec3& dims);
		Mat4& rotate(float angleRadians, const Vec3& axis);
		static Mat4 lookAt(Vec3 pos, Vec3 target, Vec3 up);
		Mat4 operator*(const Mat4& rhs) const;
	};

	struct BoundingBox
	{
		Vec3 min{ FLT_MAX, FLT_MAX, FLT_MAX };
		Vec3 max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		bool isValid{ false };

		void reset();
		void extend(const BoundingBox& other);
		void extend(const Vec3& point);
		Vec3 size() const;
		Vec3 midpoint() const;
	};

	Vec3 getSpringForce(Vec3 p1, Vec3 v1, Vec3 p2, Vec3 v2, float k, float damping, float rest);
}