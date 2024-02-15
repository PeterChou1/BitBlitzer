//---------------------------------------------------------------------------------
// Lights.h
//---------------------------------------------------------------------------------
//
// Lights used in the rendering System
//
#pragma once
#include "Quat.h"
#include "Vec3.h"

struct PointLight
{
    Vec3 Position{};
    Vec3 Color{};
    float LightPower{};
};

/// Not implemented I was planning to implement
/// Shadow mapping but ran out of time :(
struct ShadowCastingLight
{
    Quat Orientation;
    Vec3 Position;
    Vec3 Color;
};