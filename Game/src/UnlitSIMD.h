#pragma once
#include "SIMDShader.h"


class UnlitSIMD : public SIMDShader
{
    void Shade(SIMDPixel& pixel,
        std::vector<PointLight>& lights,
        Material& texture,
        Camera& cam) override;

};
