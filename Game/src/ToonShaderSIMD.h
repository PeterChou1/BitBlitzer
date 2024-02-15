//---------------------------------------------------------------------------------
// ToonShaderSIMD.h
//---------------------------------------------------------------------------------
//
// Basic Toon Shader based off of this tutorial
// https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/toon-shader-version-ii/
//
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
