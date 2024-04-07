//---------------------------------------------------------------------------------
// Triangle.h
//---------------------------------------------------------------------------------
// 
// Wrapper class for three vertices + values used to compute rasterization using
// Edge Functions
//
#pragma once

#include "Vertex.h"

struct Triangle
{
    // Vertex loaded from the obj file
    Vertex verts[3]{};

    // Precomputed values used for Edge function rasterization
    int B0{}, C0{}, B1{},
        C1{}, B2{}, C2{};

    // Indexs used to Reject or Accept a Tile
    int rejectIndex0{}, acceptIndex0{},
        rejectIndex1{}, acceptIndex1{},
        rejectIndex2{}, acceptIndex2{};

    // AABB computed for the triangle used optimization to speedup rasterization
    int maxX{}, maxY{};
    int minX{}, minY{};

    // used to retrieve vertex when interpolating pixels
    int BinID{};
    int BinIndex{};

    // Inverse Determinant Used to calculate barycentric coordinates
    float invDet{};

    Triangle() = default;

    Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) : verts{ v1, v2, v3 }
    {
    }

    CubeMapFaces GetCubmapID()
    {
        return verts[0].CubeMapID;
    }

    int GetTextureID() const
    {
        return verts[0].TextureID;
    }

    ShaderAsset GetShaderID()
    {
        return verts[0].ShaderID;
    }

    void PerspectiveDivision();


    bool Setup(int id, int index);


    float EdgeFunc0(Vec2& p) const
    {
        return B0 * (p.X - verts[0].Projection.X) - C0 * (p.Y - verts[0].Projection.Y);
    }

    float EdgeFunc1(Vec2& p) const
    {
        return B1 * (p.X - verts[1].Projection.X) - C1 * (p.Y - verts[1].Projection.Y);
    }

    float EdgeFunc2(Vec2& p) const
    {
        return B2 * (p.X - verts[2].Projection.X) - C2 * (p.Y - verts[2].Projection.Y);
    }

    bool CheckInTriangle(Vec2& point) const;
};

