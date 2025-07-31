#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float RAD2DEG = 180.f / 3.14159265358979323846f; // ~ 57.2957795131
constexpr float DEG2RAD = 3.14159265358979323846f / 180.f; // ~ 0.0174532925
constexpr float PI = 3.14159265359f;
constexpr float EPSILON = 0.000001f;

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
		bool intersects(const BoundingBox& other) const;	// box overlaps with another?
		bool contains(const Vec3& point) const;			// box has a point inside?
		bool contains(const BoundingBox& other) const;		// bounding box encloses another entirely?
	};

	Vec2 getSpringForce(Vec2 p1, Vec2 v1, Vec2 p2, Vec2 v2, float k, float damping, float rest);
	Vec3 getSpringForce(Vec3 p1, Vec3 v1, Vec3 p2, Vec3 v2, float k, float damping, float rest);

	/**
	 * @brief Transform utility.
	 *
	 * Began as just a component but has other generic uses outside of the ECS stuff
	 * @todo Methods for up/right/forward/getPosition largely untested, added due to simple copypaste port from previous (flawed) system
	 */
	struct Transform
	{
		Vec3 position{};
		Vec3 rotation{};
		Vec3 scale{ 1.f, 1.f, 1.f };

		Transform(const Vec3& position);
		Transform(const Vec3& position, const Vec3& rotation, const Vec3& scale = { 1.f, 1.f, 1.f });

		/**
		 * @brief Return the full transform matrix
		 */
		Mat4 getTransformMatrix() const;

		/**
		 * @brief Get the world position of a point
		 */
		Vec3 getPosition(const Vec3& point);

		/**
		 * @brief Get vector represeting "up"
		 */
		Vec3 up() const;

		/**
		 * @brief Get vector represeting "foward" (note: -Z forward)
		 */
		Vec3 forward() const;

		/**
		 * @brief Get vector represeting "right"
		 */
		Vec3 right() const;

		/**
		 * @brief Construct transform with starting translation
		 */
		static Transform t(const Vec3 position);

		/**
		 * @brief Construct transform with starting rotation
		 */
		static Transform r(const Vec3 rotation);

		/**
		 * @brief Construct transform with starting scale
		 */
		static Transform s(const Vec3 scale);
		static Transform s(float scale);
	};
}