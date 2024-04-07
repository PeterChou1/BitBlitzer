//---------------------------------------------------------------------------------
// BlingPhongSIMD.h
//---------------------------------------------------------------------------------
//
// A Bling Phong Shader see
// https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
// for more information
//
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