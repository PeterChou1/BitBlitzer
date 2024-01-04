#include "stdafx.h"
#include "SIMDTriangle.h"


SIMDTriangle::SIMDTriangle(const Triangle& t) : B0(SIMDFloat(static_cast<float>(t.B0))),
                                                C0(SIMDFloat(static_cast<float>(t.C0))),
                                                B1(SIMDFloat(static_cast<float>(t.B1))),
                                                C1(SIMDFloat(static_cast<float>(t.C1))),
                                                B2(SIMDFloat(static_cast<float>(t.B2))),
                                                C2(SIMDFloat(static_cast<float>(t.C2))),
                                                invDet(t.invDet),
                                                normal1(SIMDVec3(t.verts[0].normal.x, t.verts[0].normal.y, t.verts[0].normal.z)),
                                                normal2(SIMDVec3(t.verts[1].normal.x, t.verts[1].normal.y, t.verts[1].normal.z)),
                                                normal3(SIMDVec3(t.verts[2].normal.x, t.verts[2].normal.y, t.verts[2].normal.z)),
                                                v1(SIMDVec2(t.verts[0].proj.x, t.verts[0].proj.y)),
                                                v2(SIMDVec2(t.verts[1].proj.x, t.verts[1].proj.y)),
                                                v3(SIMDVec2(t.verts[2].proj.x, t.verts[2].proj.y)),
                                                invW1(t.verts[0].invW),
                                                invW2(t.verts[1].invW),
                                                invW3(t.verts[2].invW),
                                                tex1(SIMDVec2(t.verts[0].uv.x, t.verts[0].uv.y)),
                                                tex2(SIMDVec2(t.verts[1].uv.x, t.verts[1].uv.y)),
                                                tex3(SIMDVec2(t.verts[2].uv.x, t.verts[2].uv.y))
{
}


