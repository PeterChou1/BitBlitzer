#pragma once
#include "Quat.h"
#include "Vec3.h"

struct PointLight
{
    Vec3 position;
    Vec3 color;
    float lightPower;
};


struct ShadowCastingLight
{
    Quat orientation;
    Vec3 position;
    Vec3 color;
};