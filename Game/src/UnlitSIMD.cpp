#include "stdafx.h"
#include "UnlitSIMD.h"

void UnlitSIMD::Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& cam)
{
    SIMDFloat r, g, b;
    texture.SampleSIMD(pixel.textureCoord, r, g, b);
    SIMDVec3 mappedDiffuseColor = SIMDVec3(r / 255.0, g / 255.0, b / 255.0);
    pixel.color = mappedDiffuseColor;
}
