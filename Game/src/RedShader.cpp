#include "stdafx.h"
#include "RedShaderSIMD.h"

void RedShaderSIMD::Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& camera)
{
    pixel.Color = Vec3(1.0, 0.0, 0.0);
}
