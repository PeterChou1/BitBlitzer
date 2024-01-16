#include "stdafx.h"
#include "SIMDTriangle.h"


SIMDTriangle::SIMDTriangle(const Triangle& t) : B0(SIMDFloat(static_cast<float>(t.B0))),
                                                C0(SIMDFloat(static_cast<float>(t.C0))),
                                                B1(SIMDFloat(static_cast<float>(t.B1))),
                                                C1(SIMDFloat(static_cast<float>(t.C1))),
                                                B2(SIMDFloat(static_cast<float>(t.B2))),
                                                C2(SIMDFloat(static_cast<float>(t.C2))),
                                                InvDet(t.invDet),
                                                Normal1(SIMDVec3(t.verts[0].normal.X, t.verts[0].normal.Y,
                                                                 t.verts[0].normal.Z)),
                                                Normal2(SIMDVec3(t.verts[1].normal.X, t.verts[1].normal.Y,
                                                                 t.verts[1].normal.Z)),
                                                Normal3(SIMDVec3(t.verts[2].normal.X, t.verts[2].normal.Y,
                                                                 t.verts[2].normal.Z)),
                                                V1(SIMDVec2(t.verts[0].proj.X, t.verts[0].proj.Y)),
                                                V2(SIMDVec2(t.verts[1].proj.X, t.verts[1].proj.Y)),
                                                V3(SIMDVec2(t.verts[2].proj.X, t.verts[2].proj.Y)),
                                                InvW1(t.verts[0].invW),
                                                InvW2(t.verts[1].invW),
                                                InvW3(t.verts[2].invW),
                                                Tex1(SIMDVec2(t.verts[0].uv.X, t.verts[0].uv.Y)),
                                                Tex2(SIMDVec2(t.verts[1].uv.X, t.verts[1].uv.Y)),
                                                Tex3(SIMDVec2(t.verts[2].uv.X, t.verts[2].uv.Y))
{
}
