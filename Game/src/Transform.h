#pragma once
#include "Vec3.h"
#include "Quat.h"
#include "Mat4.h"


struct Transform
{
    Vec3 position;
    Quat rotation;
    Mat4 affine;
    Mat4 inverse;
    // used to update models
    bool isdirty;

    Transform();

    Transform(const Mat4& affine);

    Transform(const Vec3& pos);

    Transform(const Vec3& pos, const Quat& rot);

    Transform(const Vec3& pos, const Vec3& target, const Vec3& up);

    void Update(const Vec3& delta, const Quat& rot);

    Vec3& TransformVec3(const Vec3& point) const;

    Vec3& TransformNormal(const Vec3& normal) const;
};
