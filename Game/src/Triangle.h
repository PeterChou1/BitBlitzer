#pragma once
#include "Vertex.h"

struct Triangle
{
    Vertex verts[3]{};
    int B0{}, C0{}, B1{},
        C1{}, B2{}, C2{};

    int rejectIndex0{}, acceptIndex0{},
        rejectIndex1{}, acceptIndex1{},
        rejectIndex2{}, acceptIndex2{};

    int maxX{}, maxY{};
    int minX{}, minY{};

    // used to retrieve vertex when interpolating pixels
    int BinID{};
    int BinIndex{};

    float invDet{};

    Triangle() = default;

    Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) : verts{ v1, v2, v3 }
    {
    }

    int GetTextureID() const
    {
        return verts[0].tex_id;
    }

    ShaderAsset GetShaderID()
    {
        return verts[0].shader_id;
    }

    void PerspectiveDivision();

    bool Setup(int id, int index);

    // edge function 
    float EdgeFunc0(Vec2& p) const
    {
        return B0 * (p.X - verts[0].proj.X) - C0 * (p.Y - verts[0].proj.Y);
    }

    float EdgeFunc1(Vec2& p) const
    {
        return B1 * (p.X - verts[1].proj.X) - C1 * (p.Y - verts[1].proj.Y);
    }

    float EdgeFunc2(Vec2& p) const
    {
        return B2 * (p.X - verts[2].proj.X) - C2 * (p.Y - verts[2].proj.Y);
    }

    bool CheckInTriangle(Vec2& point) const;
};

