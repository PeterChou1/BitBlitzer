//---------------------------------------------------------------------------------
// SIMDShader.h
//---------------------------------------------------------------------------------
//
// Abstract base class for all shaders
// faster than conventional shading as
// SIMD (AVX2) allows us to shade 8 pixels at once
//
#pragma once
#include "ColorBuffer.h"
#include "Lights.h"
#include "SIMDPixel.h"
#include "Material.h"
#include "Camera.h"

class SIMDShader
{
public:
    virtual ~SIMDShader() = default;

    /**
     * \brief Abstract Shade Method called during fragment shading
     * \param pixel The sets of 8 pixels to shade
     * \param lights All lights in the scene
     * \param texture The texture belonging to the pixels
     * \param camera The current camera in the scene
     */
    virtual void Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Material& texture, Camera& camera) = 0;

};


