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
        screenSpacePosition(position), alpha(alpha), beta(beta), gamma(gamma), depth(depth), binId(binId), binIndex(binIndex)
    {
    }

    /**
     * \brief Get the texture/normal based on barycentric
     *        values computed during rasterization
     * \param t 
     */
    void Interpolate(Triangle& t)
    {
        auto t1 = SIMDVec2(t.verts[0].uv);
        auto t2 = SIMDVec2(t.verts[1].uv);
        auto t3 = SIMDVec2(t.verts[2].uv);
        auto n1 = SIMDVec3(t.verts[0].normal);
        auto n2 = SIMDVec3(t.verts[1].normal);
        auto n3 = SIMDVec3(t.verts[2].normal);
        auto p1 = SIMDVec3(t.verts[0].pos);
        auto p2 = SIMDVec3(t.verts[1].pos);
        auto p3 = SIMDVec3(t.verts[2].pos);

        textureCoord = (t1 * alpha + t2 * beta + t3 * gamma) / depth;
        normal = (n1 * alpha + n2 * beta + n3 * gamma) / depth;
        worldSpacePosition = (p1 * alpha + p2 * beta + p3 * gamma);
    }

    // static variables to tell the renderer how to screenSpacePosition each set of 8 pixels
    static constexpr int PIXEL_WIDTH = 4;
    static constexpr int PIXEL_HEIGHT = 2;
    static SIMDFloat PixelOffsetX;
    static SIMDFloat PixelOffsetY;
    // the mask indicates which pixel is to be actually shaded
    SIMDFloat mask;
    SIMDVec2 screenSpacePosition{}, textureCoord{};
    SIMDVec3 worldSpacePosition{};
    SIMDVec3 normal{};
    SIMDVec3 color{};
    // barycentric coordinates
    SIMDFloat alpha{}, beta{}, gamma{};
    // depth buffer values of current pixels
    SIMDFloat depth{};
    // this is used by the render to retrieve the triangle this pixel belong to
    int binId;
    int binIndex;
};
