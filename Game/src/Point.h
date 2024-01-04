#pragma once
#include "Vec3.h"
#include "Vec4.h"

struct Point
{
    Vec4 position{};
    Vec3 weights{};

    Point()
    {
    }

    Point(const Vec4& point, const Vec3& weights) : position(point), weights(weights)
    {
    }
};
