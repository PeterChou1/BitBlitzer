#pragma once
#include "Quat.h"
#include "Vec3.h"

struct PointLight
{
    Vec3 Position;
    Vec3 Color;
    float LightPower;
};


struct ShadowCastingLight
{
    Quat Orientation;
    Vec3 Position;
    Vec3 Color;
};