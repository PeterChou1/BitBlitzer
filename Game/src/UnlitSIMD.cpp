#include "stdafx.h"
#include "UnlitSIMD.h"

void UnlitSIMD::Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& cam)
{
    SIMDFloat r, g, b;
    texture.SampleSIMD(pixel.TextureCoord, r, g, b);
    pixel.Color = SIMDVec3(r / 255.0, g / 255.0, b / 255.0);
}
