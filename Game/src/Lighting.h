#pragma once

#include "Lights.h"
#include "Resource.h"


class Lighting : public Resource
{
public:
    void ResetResource() override
    {
        Light = {};
    }

    void SetLight(Vec3 Position, Vec3 Color, float LightPower)
    {
        Light = {Position, Color, LightPower};
    }

    PointLight GetLight()
    {
        return Light;
    }

private:
    PointLight Light;
};
