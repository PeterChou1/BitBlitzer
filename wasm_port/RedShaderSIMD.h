//---------------------------------------------------------------------------------
// RedShader.h
//---------------------------------------------------------------------------------
//
//  A shader that only outputs red
// 

#pragma once
#include "SIMDShader.h"

class RedShaderSIMD : public SIMDShader
{
public:
    void Shade(SIMDPixel& pixel,
        std::vector<PointLight>& lights,
        Material& texture,
        Camera& camera) override;

    ~RedShaderSIMD() override = default;
};