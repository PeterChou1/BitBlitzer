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

    size_t GetTextureID()
    {
        return verts[0].tex_id;
    }

    void PerspectiveDivision();

    bool Setup(int id, int index);

    // edge function 
    float EdgeFunc0(Vec2& p) const
    {
        return B0 * (p.x - verts[0].proj.x) - C0 * (p.y - verts[0].proj.y);
    }

    float EdgeFunc1(Vec2& p) const
    {
        return B1 * (p.x - verts[1].proj.x) - C1 * (p.y - verts[1].proj.y);
    }

    float EdgeFunc2(Vec2& p) const
    {
        return B2 * (p.x - verts[2].proj.x) - C2 * (p.y - verts[2].proj.y);
    }

    bool CheckInTriangle(Vec2& point) const;
};

