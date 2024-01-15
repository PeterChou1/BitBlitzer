#include "stdafx.h"
#include "Transform.h"


Transform::Transform() : Position(Vec3(0, 0, 0)), Rotation(Quat(0, 0, 0, 1))
{
    Affine.rows[0] = {1, 0, 0, 0};
    Affine.rows[1] = {0, 1, 0, 0};
    Affine.rows[2] = {0, 0, 1, 0};
    Affine.rows[3] = {0, 0, 0, 1};
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Mat4& a) : Affine(a)
{
    Position = Vec3(a[0][3], a[1][3], a[2][3]);
    Mat3 r;
    r.rows[0] = {a[0][0], a[0][1], a[0][2]};
    r.rows[1] = {a[1][0], a[1][1], a[1][2]};
    r.rows[2] = {a[2][0], a[2][1], a[2][2]};
    Rotation = Quat::FromRotationMatrix(r);
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Vec3& pos) : Position(pos), Rotation(Quat(0, 0, 0, 1))
{
    Affine.rows[0] = {1, 0, 0, pos.x};
    Affine.rows[1] = {0, 1, 0, pos.y};
    Affine.rows[2] = {0, 0, 1, pos.z};
    Affine.rows[3] = {0, 0, 0, 1};
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Vec3& pos, const Quat& rot) : Position(pos), Rotation(rot)
{
    Mat3 r = Mat3::FromQuat(rot);
    Affine.rows[0] = {r[0][0], r[0][1], r[0][2], pos.x};
    Affine.rows[1] = {r[1][0], r[1][1], r[1][2], pos.y};
    Affine.rows[2] = {r[2][0], r[2][1], r[2][2], pos.z};
    Affine.rows[3] = {0.0, 0.0, 0.0, 1.0};
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Vec3& pos, const Vec3& target, const Vec3& up)
{
    Vec3 forward = (target - pos).Normalize() * -1;
    Vec3 right = up.Cross(forward).Normalize();
    Vec3 camUp = forward.Cross(right);
    Mat3 r;
    r.rows[0] = {right.x, camUp.x, forward.x};
    r.rows[1] = {right.y, camUp.y, forward.y};
    r.rows[2] = {right.z, camUp.z, forward.z};
    Position = pos;
    Rotation = Quat::FromRotationMatrix(r);
    Affine.rows[0] = {right.x, camUp.x, forward.x, pos.x};
    Affine.rows[1] = {right.y, camUp.y, forward.y, pos.y};
    Affine.rows[2] = {right.z, camUp.z, forward.z, pos.z};
    Affine.rows[3] = {0.0, 0.0, 0.0, 1.0};
    Inverse = Affine.AffineInverse();
}

void Transform::SetPosition(const Vec3& pos)
{
    Position = pos;
    Affine.rows[0][3] = Position.x;
    Affine.rows[1][3] = Position.y;
    Affine.rows[2][3] = Position.z;
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::SetPosition2D(const Vec2& pos)
{
    Position.x = pos.x;
    Position.y = pos.y;
    Affine.rows[0][3] = Position.x;
    Affine.rows[1][3] = Position.y;
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdateRow(float row)
{
    Quat q = Quat(Vec3(1, 0, 0), row);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.rows[0] = { newrot[0][0], newrot[0][1], newrot[0][2], Position.x };
    Affine.rows[1] = { newrot[1][0], newrot[1][1], newrot[1][2], Position.y };
    Affine.rows[2] = { newrot[2][0], newrot[2][1], newrot[2][2], Position.z };
    Affine.rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdatePitch(float pitch)
{
    Quat q = Quat(Vec3(0, 1, 0), pitch);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.rows[0] = { newrot[0][0], newrot[0][1], newrot[0][2], Position.x };
    Affine.rows[1] = { newrot[1][0], newrot[1][1], newrot[1][2], Position.y };
    Affine.rows[2] = { newrot[2][0], newrot[2][1], newrot[2][2], Position.z };
    Affine.rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdateYaw(float yaw)
{
    Quat q = Quat(Vec3(0, 0, 1), yaw);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.rows[0] = { newrot[0][0], newrot[0][1], newrot[0][2], Position.x };
    Affine.rows[1] = { newrot[1][0], newrot[1][1], newrot[1][2], Position.y };
    Affine.rows[2] = { newrot[2][0], newrot[2][1], newrot[2][2], Position.z };
    Affine.rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::Update(const Vec3& delta, const Quat& rot)
{
    Rotation = Rotation * rot;
    Position = Position + delta;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.rows[0] = {newrot[0][0], newrot[0][1], newrot[0][2], Position.x};
    Affine.rows[1] = {newrot[1][0], newrot[1][1], newrot[1][2], Position.y};
    Affine.rows[2] = {newrot[2][0], newrot[2][1], newrot[2][2], Position.z};
    Affine.rows[3] = {0.0, 0.0, 0.0, 1.0};
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}


Vec3 Transform::TransformVec3(const Vec3& point) const
{
    return Affine * point;
}

Vec3 Transform::TransformNormal(const Vec3& normal) const
{
    return Rotation.RotatePoint(normal);
}
