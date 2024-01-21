#include "stdafx.h"

#include "Transform.h"


Transform::Transform() : Position(Vec3(0, 0, 0)), Rotation(Quat(0, 0, 0, 1))
{
    Affine.Rows[0] = {1, 0, 0, 0};
    Affine.Rows[1] = {0, 1, 0, 0};
    Affine.Rows[2] = {0, 0, 1, 0};
    Affine.Rows[3] = {0, 0, 0, 1};
    TransformScale = 1.0;
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Mat4& a) : Affine(a)
{
    Position = Vec3(a[0][3], a[1][3], a[2][3]);
    Mat3 r;
    r.Rows[0] = {a[0][0], a[0][1], a[0][2]};
    r.Rows[1] = {a[1][0], a[1][1], a[1][2]};
    r.Rows[2] = {a[2][0], a[2][1], a[2][2]};
    Rotation = Quat::FromRotationMatrix(r);
    TransformScale = 1.0;
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Vec3& pos) : Position(pos), Rotation(Quat(0, 0, 0, 1))
{
    Affine.Rows[0] = {1, 0, 0, pos.X};
    Affine.Rows[1] = {0, 1, 0, pos.Y};
    Affine.Rows[2] = {0, 0, 1, pos.Z};
    Affine.Rows[3] = {0, 0, 0, 1};
    TransformScale = 1.0;
    Inverse = Affine.AffineInverse();
}

Transform::Transform(const Vec3& pos, const Quat& rot) : Position(pos), Rotation(rot)
{
    Mat3 r = Mat3::FromQuat(rot);
    Affine.Rows[0] = {r[0][0], r[0][1], r[0][2], pos.X};
    Affine.Rows[1] = {r[1][0], r[1][1], r[1][2], pos.Y};
    Affine.Rows[2] = {r[2][0], r[2][1], r[2][2], pos.Z};
    Affine.Rows[3] = {0.0, 0.0, 0.0, 1.0};
    TransformScale = 1.0;
    Inverse = Affine.AffineInverse();
}


void Transform::Scale(float scale)
{
    Affine.Rows[0] = { scale * Affine.Rows[0][0], scale * Affine.Rows[0][1], scale * Affine.Rows[0][2], Position.X };
    Affine.Rows[1] = { scale * Affine.Rows[1][0], scale * Affine.Rows[1][1], scale * Affine.Rows[1][2], Position.Y };
    Affine.Rows[2] = { scale * Affine.Rows[2][0], scale * Affine.Rows[2][1], scale * Affine.Rows[2][2], Position.Z };
    Affine.Rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
    TransformScale *= scale;
}

Transform::Transform(const Vec3& pos, const Vec3& target, const Vec3& up)
{
    Vec3 forward = (target - pos).Normalize() * -1;
    Vec3 right = up.Cross(forward).Normalize();
    Vec3 camUp = forward.Cross(right);
    Mat3 r;
    r.Rows[0] = {right.X, camUp.X, forward.X};
    r.Rows[1] = {right.Y, camUp.Y, forward.Y};
    r.Rows[2] = {right.Z, camUp.Z, forward.Z};
    Position = pos;
    Rotation = Quat::FromRotationMatrix(r);
    Affine.Rows[0] = {right.X, camUp.X, forward.X, pos.X};
    Affine.Rows[1] = {right.Y, camUp.Y, forward.Y, pos.Y};
    Affine.Rows[2] = {right.Z, camUp.Z, forward.Z, pos.Z};
    Affine.Rows[3] = {0.0, 0.0, 0.0, 1.0};
    TransformScale = 1.0;
    Inverse = Affine.AffineInverse();
}

Vec3 Transform::GetForward()
{
    return {Affine[0][2], Affine[1][2], Affine[2][2]};
}

void Transform::SetPosition(const Vec3& pos)
{
    Position = pos;
    Affine.Rows[0][3] = Position.X;
    Affine.Rows[1][3] = Position.Y;
    Affine.Rows[2][3] = Position.Z;
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::SetPosition2D(const Vec2& pos)
{
    Position.X = pos.X;
    Position.Y = pos.Y;
    Affine.Rows[0][3] = Position.X;
    Affine.Rows[1][3] = Position.Y;
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdateRow(float row)
{
    Quat q = Quat(Vec3(1, 0, 0), row);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.Rows[0] = { TransformScale * newrot[0][0], TransformScale * newrot[0][1], TransformScale * newrot[0][2], Position.X };
    Affine.Rows[1] = { TransformScale * newrot[1][0], TransformScale * newrot[1][1], TransformScale * newrot[1][2], Position.Y };
    Affine.Rows[2] = { TransformScale * newrot[2][0], TransformScale * newrot[2][1], TransformScale * newrot[2][2], Position.Z };
    Affine.Rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdatePitch(float pitch)
{
    Quat q = Quat(Vec3(0, 1, 0), pitch);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.Rows[0] = { TransformScale * newrot[0][0], TransformScale * newrot[0][1], TransformScale * newrot[0][2], Position.X };
    Affine.Rows[1] = { TransformScale * newrot[1][0], TransformScale * newrot[1][1], TransformScale * newrot[1][2], Position.Y };
    Affine.Rows[2] = { TransformScale * newrot[2][0], TransformScale * newrot[2][1], TransformScale * newrot[2][2], Position.Z };
    Affine.Rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::UpdateYaw(float yaw)
{
    Quat q = Quat(Vec3(0.0f, 0.0f, 1.0), yaw);
    Rotation *= q;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.Rows[0] = { TransformScale * newrot[0][0], TransformScale * newrot[0][1], TransformScale * newrot[0][2], Position.X };
    Affine.Rows[1] = { TransformScale * newrot[1][0], TransformScale * newrot[1][1], TransformScale * newrot[1][2], Position.Y };
    Affine.Rows[2] = { TransformScale * newrot[2][0], TransformScale * newrot[2][1], TransformScale * newrot[2][2], Position.Z };
    Affine.Rows[3] = { 0.0, 0.0, 0.0, 1.0 };
    Inverse = Affine.AffineInverse();
    IsDirty = true;
}

void Transform::Update(const Vec3& delta, const Quat& rot)
{
    Rotation = Rotation * rot;
    Position = Position + delta;
    Mat3 newrot = Mat3::FromQuat(Rotation);
    Affine.Rows[0] = { TransformScale * newrot[0][0], TransformScale * newrot[0][1], TransformScale * newrot[0][2], Position.X };
    Affine.Rows[1] = { TransformScale * newrot[1][0], TransformScale * newrot[1][1], TransformScale * newrot[1][2], Position.Y };
    Affine.Rows[2] = { TransformScale * newrot[2][0], TransformScale * newrot[2][1], TransformScale * newrot[2][2], Position.Z };
    Affine.Rows[3] = {0.0, 0.0, 0.0, 1.0};
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
