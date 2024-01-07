#pragma once
#include "ColorBuffer.h"
#include "Lights.h"
#include "SIMDPixel.h"
#include "Texture.h"

class SIMDShader
{
public:
    virtual ~SIMDShader() = default;

    virtual void Shade(SIMDPixel& pixel, std::vector<PointLight>& lights, Texture& texture, Camera& camera) = 0;

    static void SetColorBuffer(SIMDPixel& pixel, ColorBuffer& colorBuffer)
    {
        SIMDVec3 color = pixel.color * 255.0;

        color.x = color.x.floor();
        color.y = color.y.floor();
        color.z = color.z.floor();

        if (pixel.mask.GetBit(0))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[0], pixel.screenSpacePosition.y[0], color.x[0], color.y[0], color.z[0]);
        }

        if (pixel.mask.GetBit(1))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[1], pixel.screenSpacePosition.y[1], color.x[1], color.y[1], color.z[1]);
        }

        if (pixel.mask.GetBit(2))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[2], pixel.screenSpacePosition.y[2], color.x[2], color.y[2], color.z[2]);
        }

        if (pixel.mask.GetBit(3))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[3], pixel.screenSpacePosition.y[3], color.x[3], color.y[3], color.z[3]);
        }

        if (pixel.mask.GetBit(4))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[4], pixel.screenSpacePosition.y[4], color.x[4], color.y[4], color.z[4]);
        }

        if (pixel.mask.GetBit(5))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[5], pixel.screenSpacePosition.y[5], color.x[5], color.y[5], color.z[5]);
        }

        if (pixel.mask.GetBit(6))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[6], pixel.screenSpacePosition.y[6], color.x[6], color.y[6], color.z[6]);
        }

        if (pixel.mask.GetBit(7))
        {
            colorBuffer.SetColor(pixel.screenSpacePosition.x[7], pixel.screenSpacePosition.y[7], color.x[7], color.y[7], color.z[7]);
        }
    }
};


