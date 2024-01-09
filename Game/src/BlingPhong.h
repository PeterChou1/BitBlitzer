#pragma once
#include "SIMDShader.h"

class BlinnPhongSIMD : public SIMDShader
{
public:
    void Shade(SIMDPixel& pixel, 
               std::vector<PointLight>& lights,
               Material& texture,
               Camera& camera) override;

    ~BlinnPhongSIMD() override = default;
};