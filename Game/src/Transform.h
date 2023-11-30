#pragma once
#include "Vec3.h"
#include "Quat.h"


struct Transform {
	Vec3 position;
	Quat rotation;

	Transform();

	Transform(const Vec3& pos, const Quat& rot);

	Transform(const Vec3& pos, const Vec3& target, const Vec3& up);

	Transform Inverse();
};