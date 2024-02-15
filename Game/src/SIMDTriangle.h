//---------------------------------------------------------------------------------
// SIMDTriangle.h
//---------------------------------------------------------------------------------
//
// SIMDTriangle wraps around a normal triangle this is simply to make it
// easier to work with SIMD operations during rasterization see comments
// in Triangle.h for more explanation on which each fields mean
//
#pragma once
#include "SIMD.h"
#include "Triangle.h"

struct SIMDTriangle
{
    SIMDFloat B0{}, C0{}, B1{}, C1{}, B2{}, C2{};
    SIMDFloat InvDet;
    SIMDVec3 Normal1, Normal2, Normal3;
    SIMDVec2 V1, V2, V3;
    SIMDFloat InvW1, InvW2, InvW3;
    SIMDVec2 Tex1, Tex2, Tex3;

    SIMDTriangle(const Triangle& t);

    SIMDFloat EdgeFunc0(SIMDVec2& p) const
    {
        return B0 * (p.X - V1.X) - C0 * (p.Y - V1.Y);
    }

    SIMDFloat EdgeFunc1(SIMDVec2& p) const
    {
        return B1 * (p.X - V2.X) - C1 * (p.Y - V2.Y);
    }

    SIMDFloat EdgeFunc2(SIMDVec2& p) const
    {
        return B2 * (p.X - V3.X) - C2 * (p.Y - V3.Y);
    }

    void ComputeBarycentric(SIMDFloat& x, SIMDFloat& y, SIMDFloat& alpha, SIMDFloat& beta, SIMDFloat& gamma) const
    {
        alpha = (B1 * -1 * (x - V3.X) + C1 * (y - V3.Y)) * InvDet;
        beta = (B2 * -1 * (x - V3.X) + C2 * (y - V3.Y)) * InvDet;
        gamma = SIMDFloat(1.0) - alpha - beta;
    }
};
