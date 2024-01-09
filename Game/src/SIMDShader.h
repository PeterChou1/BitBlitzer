#pragma once
#include "ColorBuffer.h"
#include "Lights.h"
#include "SIMDPixel.h"
#include "Material.h"
#include "Camera.h"

class SIMDShader
{
public:
    virtual ~SIMDShader() = default;

    virtual void Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& camera) = 0;

};


