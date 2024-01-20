#pragma once

#include "Vec3.h"

struct GameState : Resource
{
    Vec3 LaunchPoint{};

    float MaxPower = 500.0f;

    void ResetResource() override {}
};
