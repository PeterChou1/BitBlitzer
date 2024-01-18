//---------------------------------------------------------------------------------
// SIMDPixel.h
//---------------------------------------------------------------------------------
//
// SIMDPixel is a wrapper class around sets of 8 pixels
//
#pragma once
#include "SIMD.h"
#include "Triangle.h"


class SIMDPixel
{
public:
    SIMDPixel(const SIMDVec2& position, const SIMDFloat& depth, const SIMDFloat& alpha, const SIMDFloat& beta,
              const SIMDFloat& gamma,
              int binId, int binIndex) :
        ScreenSpacePosition(position), Alpha(alpha), Beta(beta), Gamma(gamma), Depth(depth), BinId(binId),
        BinIndex(binIndex)
    {
    }

    /**
     * \brief Get the texture/normal based on barycentric
     *        values computed during rasterization
     * \param t 
     */
    void Interpolate(Triangle& t)
    {
        auto t1 = SIMDVec2(t.verts[0].UV);
        auto t2 = SIMDVec2(t.verts[1].UV);
        auto t3 = SIMDVec2(t.verts[2].UV);
        auto n1 = SIMDVec3(t.verts[0].Normal);
        auto n2 = SIMDVec3(t.verts[1].Normal);
        auto n3 = SIMDVec3(t.verts[2].Normal);
        auto p1 = SIMDVec3(t.verts[0].Position);
        auto p2 = SIMDVec3(t.verts[1].Position);
        auto p3 = SIMDVec3(t.verts[2].Position);

        TextureCoord = (t1 * Alpha + t2 * Beta + t3 * Gamma) / Depth;
        Normal = (n1 * Alpha + n2 * Beta + n3 * Gamma) / Depth;
        WorldSpacePosition = (p1 * Alpha + p2 * Beta + p3 * Gamma);
    }

    // static variables to tell the renderer how to screenSpacePosition each set of 8 pixels
    static constexpr int PIXEL_WIDTH = 4;
    static constexpr int PIXEL_HEIGHT = 2;
    static SIMDFloat PixelOffsetX;
    static SIMDFloat PixelOffsetY;
    // the mask indicates which pixel is to be actually shaded
    SIMDFloat Mask;
    SIMDVec2 ScreenSpacePosition{}, TextureCoord{};
    SIMDVec3 WorldSpacePosition{};
    SIMDVec3 Normal{};
    SIMDVec3 Color{};
    // barycentric coordinates
    SIMDFloat Alpha{}, Beta{}, Gamma{};
    // depth buffer values of current pixels
    SIMDFloat Depth{};
    // this is used by the render to retrieve the triangle this pixel belong to
    int BinId;
    int BinIndex;
};
