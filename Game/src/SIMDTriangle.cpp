#include "stdafx.h"
#include "SIMDTriangle.h"


SIMDTriangle::SIMDTriangle(const Triangle& t) : B0(SIMDFloat(static_cast<float>(t.B0))),
                                                C0(SIMDFloat(static_cast<float>(t.C0))),
                                                B1(SIMDFloat(static_cast<float>(t.B1))),
                                                C1(SIMDFloat(static_cast<float>(t.C1))),
                                                B2(SIMDFloat(static_cast<float>(t.B2))),
                                                C2(SIMDFloat(static_cast<float>(t.C2))),
                                                InvDet(t.invDet),
                                                Normal1(SIMDVec3(t.verts[0].Normal.X, t.verts[0].Normal.Y,
                                                                 t.verts[0].Normal.Z)),
                                                Normal2(SIMDVec3(t.verts[1].Normal.X, t.verts[1].Normal.Y,
                                                                 t.verts[1].Normal.Z)),
                                                Normal3(SIMDVec3(t.verts[2].Normal.X, t.verts[2].Normal.Y,
                                                                 t.verts[2].Normal.Z)),
                                                V1(SIMDVec2(t.verts[0].Projection.X, t.verts[0].Projection.Y)),
                                                V2(SIMDVec2(t.verts[1].Projection.X, t.verts[1].Projection.Y)),
                                                V3(SIMDVec2(t.verts[2].Projection.X, t.verts[2].Projection.Y)),
                                                InvW1(t.verts[0].InverseW),
                                                InvW2(t.verts[1].InverseW),
                                                InvW3(t.verts[2].InverseW),
                                                Tex1(SIMDVec2(t.verts[0].UV.X, t.verts[0].UV.Y)),
                                                Tex2(SIMDVec2(t.verts[1].UV.X, t.verts[1].UV.Y)),
                                                Tex3(SIMDVec2(t.verts[2].UV.X, t.verts[2].UV.Y))
{
}
