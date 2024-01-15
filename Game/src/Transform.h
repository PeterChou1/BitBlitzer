#pragma once

#include "Vec3.h"
#include "Quat.h"
#include "Mat4.h"


struct Transform
{
    Vec3 Position;
    Quat Rotation;
    Mat4 Affine;
    Mat4 Inverse;
    bool IsDirty{};

    Transform();

    Transform(const Mat4& affine);

    Transform(const Vec3& pos);

    Transform(const Vec3& pos, const Quat& rot);

    Transform(const Vec3& pos, const Vec3& target, const Vec3& up);

    void SetPosition(const Vec3& pos);

    void SetPosition2D(const Vec2& pos);

    void UpdateRow(float row);

    void UpdatePitch(float pitch);

    void UpdateYaw(float yaw);

    void Update(const Vec3& delta, const Quat& rot);

    Vec3 TransformVec3(const Vec3& point) const;

    Vec3 TransformNormal(const Vec3& normal) const;
};
