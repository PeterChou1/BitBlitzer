#pragma once
#include "SIMD.h"
#include "Triangle.h"

struct SIMDTriangle
{
    SIMDFloat B0{}, C0{}, B1{}, C1{}, B2{}, C2{};
    SIMDFloat invDet;
    SIMDVec3 normal1, normal2, normal3;
    SIMDVec2 v1, v2, v3;
    SIMDFloat invW1, invW2, invW3;
    SIMDVec2 tex1, tex2, tex3;

    SIMDTriangle(const Triangle& t);

    SIMDFloat EdgeFunc0(SIMDVec2& p) const
    {
        return B0 * (p.x - v1.x) - C0 * (p.y - v1.y);
    }

    SIMDFloat EdgeFunc1(SIMDVec2& p) const
    {
        return B1 * (p.x - v2.x) - C1 * (p.y - v2.y);
    };

    SIMDFloat EdgeFunc2(SIMDVec2& p) const
    {
        return B2 * (p.x - v3.x) - C2 * (p.y - v3.y);
    }

    void ComputeBarycentric(SIMDFloat& x, SIMDFloat& y, SIMDFloat& alpha, SIMDFloat& beta, SIMDFloat& gamma) const
    {
        alpha = (B1 * -1 * (x - v3.x) + C1 * (y - v3.y)) * invDet;
        beta = (B2 * -1 * (x - v3.x) + C2 * (y - v3.y)) * invDet;
        gamma = SIMDFloat(1.0) - alpha - beta;
    }
};
