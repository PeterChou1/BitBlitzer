#pragma once
#include "Camera.h"
#include "SIMDShader.h"

class BlingPhongSIMD : public SIMDShader
{
public:
    void Shade(SIMDPixel& pixel, 
               std::vector<PointLight>& lights,
               Texture& texture,
               Camera& camera) override;

    ~BlingPhongSIMD() override = default;
};