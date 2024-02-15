//---------------------------------------------------------------------------------
// UnlitSIMD.h
//---------------------------------------------------------------------------------
//
// Basic Unlit Shader shades the color based off the texture
// removes all lighting calculation
//
#pragma once
#include "SIMDShader.h"


class UnlitSIMD : public SIMDShader
{
    void Shade(SIMDPixel& pixel,
        std::vector<PointLight>& lights,
        Material& texture,
        Camera& cam) override;

};
