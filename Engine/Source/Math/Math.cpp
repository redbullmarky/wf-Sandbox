#include "pch.h"
#include "Math/Math.h"

namespace wf
{
	Mat4& Mat4::translate(const Vec3& pt)
	{
		matrix = glm::translate(matrix, pt);
		return *this;
	}

	Mat4& Mat4::scale(const Vec3& dims)
	{
		matrix = glm::scale(matrix, dims);
		return *this;
	}

	Mat4& Mat4::rotate(float angleRadians, const Vec3& axis)
	{
		matrix = glm::rotate(matrix, angleRadians, axis);
		return *this;
	}

	Mat4 Mat4::lookAt(Vec3 pos, Vec3 target, Vec3 up)
	{
		return Mat4{ glm::lookAt(pos, target, up) };
	}

	Mat4 Mat4::operator*(const Mat4& rhs) const
	{
		return Mat4{ matrix * rhs.matrix };
	}

	void BoundingBox::reset()
	{
		min = { FLT_MAX, FLT_MAX, FLT_MAX };
		max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		isValid = false;
	}

	void BoundingBox::extend(const BoundingBox& other)
	{
		extend(other.min);
		extend(other.max);
	}

	void BoundingBox::extend(const Vec3& point)
	{
		if (point.x < min.x) min.x = point.x;
		if (point.y < min.y) min.y = point.y;
		if (point.z < min.z) min.z = point.z;
		if (point.x > max.x) max.x = point.x;
		if (point.y > max.y) max.y = point.y;
		if (point.z > max.z) max.z = point.z;
		isValid = min != max;
	}

	Vec3 BoundingBox::size() const
	{
		return max - min;
	}

	Vec3 BoundingBox::midpoint() const
	{
		return (min + max) * .5f;
	}

	bool BoundingBox::intersects(const BoundingBox& other) const
	{
		if (!isValid) return false;
		bool overlapX = ((min.x <= other.max.x) && (max.x >= other.min.x));
		bool overlapY = ((min.y <= other.max.y) && (max.y >= other.min.y));
		bool overlapZ = ((min.z <= other.max.z) && (max.z >= other.min.z));
		return (overlapX && overlapY && overlapZ);
	}

	bool BoundingBox::contains(const Vec3& point) const
	{
		if (!isValid) return false;
		return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z && point.z <= max.z;
	}

	bool BoundingBox::contains(const BoundingBox& other) const
	{
		return contains(other.min) && contains(other.max);
	}

	Transform::Transform(const Vec3& position)
		: position(position), rotation(Vec3{}) {
	}

	Transform::Transform(const Vec3& position, const Vec3& rotation, const Vec3& scale)
		: position(position), rotation(rotation), scale(scale) {
	}

	Mat4 Transform::getTransformMatrix() const
	{
		auto transform = Mat4(1.f);

		transform.translate(position);

		transform.rotate(rotation.y * DEG2RAD, Vec3(0, 1, 0));
		transform.rotate(rotation.x * DEG2RAD, Vec3(1, 0, 0));
		transform.rotate(rotation.z * DEG2RAD, Vec3(0, 0, 1));

		transform.scale(scale);

		return transform;
	}

	Vec3 Transform::getWorldPosition(const Vec3& point) const
	{
		auto m = getTransformMatrix().matrix;
		glm::vec4 world = m * glm::vec4(point, 1.f);
		return Vec3(world);
	}

	Vec3 Transform::up() const
	{
		return Vec3(getTransformMatrix().matrix[1]); // column 1
	}

	Vec3 Transform::forward() const
	{
		return -Vec3(getTransformMatrix().matrix[2]); // column 2, NEGATED for -Z forward
	}

	Vec3 Transform::right() const
	{
		return Vec3(getTransformMatrix().matrix[0]); // column 0
	}

	Transform Transform::t(const Vec3& position)
	{
		return Transform(position);
	}

	Transform Transform::r(const Vec3& rotation)
	{
		return Transform(Vec3{}, rotation);
	}

	Transform Transform::s(const Vec3& scale)
	{
		return Transform(Vec3{}, Vec3{}, scale);
	}

	Transform Transform::s(float scale)
	{
		return s(Vec3{ scale, scale, scale });
	}
}