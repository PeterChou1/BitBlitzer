#pragma once
#include "SIMDShader.h"

class ToonShaderSIMD : public SIMDShader
{
public:
    void Shade(SIMDPixel& pixel,
        std::vector<PointLight>& lights,
        Material& texture,
        Camera& camera) override;

    ~ToonShaderSIMD() override = default;
};
