#pragma once
#include "SIMD.h"
#include "Triangle.h"

/*
* /brief sets of 8 pixels (4 x 2)
*/
class SIMDPixel
{
public:
    SIMDPixel(const SIMDVec2& position, const SIMDFloat& depth, SIMDFloat alpha, SIMDFloat beta, SIMDFloat gamma,
        int binId, int binIndex) :
        position(position), alpha(alpha), beta(beta), gamma(gamma), depth(depth), binId(binId), binIndex(binIndex)
    {
    }

    void Interpolate(Triangle& t)
    {
        auto t1 = SIMDVec2(t.verts[0].uv);
        auto t2 = SIMDVec2(t.verts[1].uv);
        auto t3 = SIMDVec2(t.verts[2].uv);
        auto n1 = SIMDVec3(t.verts[0].normal);
        auto n2 = SIMDVec3(t.verts[1].normal);
        auto n3 = SIMDVec3(t.verts[2].normal);
        textureCoord = (t1 * alpha + t2 * beta + t3 * gamma) / depth;
        normal = (n1 * alpha + n2 * beta + n3 * gamma) / depth;
    }

    static constexpr int PIXEL_WIDTH = 4;
    static constexpr int PIXEL_HEIGHT = 2;
    static SIMDFloat PixelOffsetX;
    static SIMDFloat PixelOffsetY;
    SIMDFloat mask;
    SIMDVec2 position{}, textureCoord{};
    SIMDVec3 normal{};
    // barycentric coordinates
    SIMDFloat alpha{}, beta{}, gamma{};
    // depth buffer values of current pixels
    SIMDFloat depth{};
    int binId;
    int binIndex;
};
