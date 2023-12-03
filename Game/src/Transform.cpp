#include "stdafx.h"
#include "Transform.h"


Transform::Transform() : position(Vec3(0, 0, 0)), rotation(Quat(0, 0, 0, 1)) 
{
	affine.rows[0] = {1, 0, 0, 0};
	affine.rows[1] = {0, 1, 0, 0};
	affine.rows[2] = {0, 0, 1, 0};
	affine.rows[3] = {0, 0, 0, 1};
}

Transform::Transform(const Mat4& a) : affine(a)
{
	position = Vec3(a[0][3], a[1][3], a[2][3]);
	Mat3 r;
	r.rows[0] = { a[0][0], a[0][1], a[0][2]};
	r.rows[1] = { a[1][0], a[1][1], a[1][2]};
	r.rows[2] = { a[2][0], a[2][1], a[2][2]};
	rotation = Quat::FromRotationMatrix(r);
}

Transform::Transform(const Vec3& pos) : position(pos), rotation(Quat(0, 0, 0, 1)) 
{
	affine.rows[0] = { 1, 0, 0, pos.x };
	affine.rows[1] = { 0, 1, 0, pos.y };
	affine.rows[2] = { 0, 0, 1, pos.z };
	affine.rows[3] = { 0, 0, 0, 1 };
}

Transform::Transform(const Vec3& pos, const Quat& rot) : position(pos), rotation(rot) 
{
	Mat3 r = Mat3::FromQuat(rot);
	affine.rows[0] = { r[0][0], r[0][1], r[0][2], pos.x};
	affine.rows[1] = { r[1][0], r[1][1], r[1][2], pos.y };
	affine.rows[2] = { r[2][0], r[2][1], r[2][2], pos.z };
	affine.rows[3] = { 0.0, 0.0, 0.0, 1.0 };

}

Transform::Transform(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	Vec3 forward = (pos - target).Normalize();
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
	return Transform(affine.AffineInverse());
}
