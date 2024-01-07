#pragma once
#include "Camera.h"
#include "SIMDShader.h"

class BlinnPhongSIMD : public SIMDShader
{
public:
    void Shade(SIMDPixel& pixel, 
               std::vector<PointLight>& lights,
               Texture& texture,
               Camera& camera) override;

    ~BlinnPhongSIMD() override = default;
};