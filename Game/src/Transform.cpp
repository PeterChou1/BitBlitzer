#include "stdafx.h"
#include "Transform.h"


Transform::Transform() : position(Vec3(0, 0, 0)), rotation(Quat(0, 0, 0, 1)) 
{
	affine.rows[0] = {1, 0, 0, 0};
	affine.rows[1] = {0, 1, 0, 0};
	affine.rows[2] = {0, 0, 1, 0};
	affine.rows[3] = {0, 0, 0, 1};
}

Transform::Transform(const Vec3& pos) : position(pos), rotation(Quat(0, 0, 0, 1)) 
{
	affine.rows[0] = { 1, 0, 0, pos.x };
	affine.rows[1] = { 0, 1, 0, pos.y };
	affine.rows[2] = { 0, 0, 1, pos.z };
	affine.rows[3] = { 0, 0, 0, 1 };
}

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

	affine.rows[0] = { right.x, camUp.x, forward.x, pos.x };
	affine.rows[1] = { right.y, camUp.y, forward.y, pos.y };
	affine.rows[2] = { right.z, camUp.z, forward.z, pos.z };
	affine.rows[3] = { 0.0, 0.0, 0.0, 1.0 };
}

Transform Transform::Inverse()
{

	Transform t = Transform();
	t.affine = affine.Inverse();
	return t;
}
