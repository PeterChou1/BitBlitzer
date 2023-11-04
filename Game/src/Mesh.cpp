#include "stdafx.h"
#include "Mesh.h"

const Triangle Triangle::operator*(const Transform& rhs)
{
	Triangle tri = *this;
	tri.tri[0] *= rhs;
	tri.tri[1] *= rhs;
	tri.tri[2] *= rhs;
	return tri;
}

Transform::Transform() : position(Vec3(0,0,0)), rotation(Quat(0, 0, 0, 1)) {}

Transform::Transform(const Vec3& pos, const Quat& rot) : position(pos), rotation(rot) {}

Transform::Transform(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	Vec3 forward = (target - pos).Normalize();
	Vec3 right = up.Cross(forward).Normalize();
	Vec3 camUp = forward.Cross(right);
	Mat3 r;
	r.rows[0] = { right.x, camUp.x, forward.x };
	r.rows[1] = { right.y, camUp.y, forward.y };
	r.rows[2] = { right.z, camUp.z, forward.z };

	position = pos;
	rotation = Quat::FromRotationMatrix(r);
}

Transform Transform::Inverse()
{
	return Transform(position * -1.0, rotation.Inverse());
}
