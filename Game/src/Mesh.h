#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include <vector>
#include "Quat.h"
#include "Transform.h"
#include "Entity.h"
#include "SIMD.h"

struct Vertex
{
    // parent entity this belongs to
    size_t tex_id{};

    std::uint32_t index{};
    Vec2 tex{};
    Vec3 normal{};
    // camera space position
    Vec3 pos{};
    // 4d homogenous coordiates output by vertex shader 
    Vec4 proj{};
    // inverse w used for perspective corrected interpolation
    float invW{};

    Vertex()
    {
    }

    Vertex(Vec3& pos) : pos(pos)
    {
    }

    Vertex(Vec3& pos, Vec2& tex) : pos(pos), tex(tex)
    {
    }

    Vertex(Vec3& pos, Vec3& normal, Vec2& tex) : pos(pos), normal(normal), tex(tex)
    {
    }

    Vertex(const Vertex& v)
        : tex_id(v.tex_id)
          , index(v.index)
          , tex(v.tex)
          , normal(v.normal)
          , pos(v.pos)
          , proj(v.proj)
          , invW(v.invW)
    {
    }

    void PerspectiveDivision()
    {
        invW = 1 / proj.w;
        proj *= invW;
        tex *= invW;
    }

    Vertex operator*(float t) const
    {
        auto copy = *this;
        copy.pos *= t;
        copy.tex *= t;
        copy.normal *= t;
        return copy;
    }


    Vertex operator+(const Vertex& v) const
    {
        auto copy = *this;

        copy.pos += v.pos;
        copy.tex += v.tex;
        copy.normal += v.normal;

        return copy;
    }


    bool operator==(const Vertex& rhs) const
    {
        return tex == rhs.tex && normal == rhs.normal &&
            pos == rhs.pos && invW == rhs.invW && tex_id == rhs.tex_id;
    }
};


// hash method used: https://en.cppreference.com/w/Talk:cpp/utility/hash
template <>
struct std::hash<Vertex>
{
    std::size_t operator()(Vertex const& v) const noexcept
    {
        std::size_t h1 = std::hash<Entity>{}(v.tex_id);
        h1 = h1 ^ (std::hash<float>{}(v.tex.x) << 1);
        h1 = h1 ^ (std::hash<float>{}(v.tex.y) << 2);
        h1 = h1 ^ (std::hash<float>{}(v.pos.x) << 3);
        h1 = h1 ^ (std::hash<float>{}(v.pos.y) << 4);
        h1 = h1 ^ (std::hash<float>{}(v.pos.z) << 5);
        h1 = h1 ^ (std::hash<float>{}(v.normal.x) << 6);
        h1 = h1 ^ (std::hash<float>{}(v.normal.y) << 7);
        return h1;
    }
};


struct Triangle
{
    Vertex verts[3];
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

    Triangle(const Triangle& t) :
        B0(t.B0), C0(t.C0), B1(t.B1),
        C1(t.C1), B2(t.B2), C2(t.C2),
        rejectIndex0(t.rejectIndex0), acceptIndex0(t.acceptIndex0),
        rejectIndex1(t.rejectIndex1), acceptIndex1(t.acceptIndex1),
        rejectIndex2(t.rejectIndex2), acceptIndex2(t.acceptIndex2),
        maxX(t.maxX), maxY(t.maxY),
        minX(t.minX), minY(t.minY), invDet(t.invDet)
    {
        std::copy(std::begin(t.verts), std::end(t.verts), std::begin(verts));
    }

    Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) : verts{v1, v2, v3}
    {
    }

    Triangle() : verts{}
    {
    }

    void PerspectiveDivision()
    {
        verts[0].PerspectiveDivision();
        verts[1].PerspectiveDivision();
        verts[2].PerspectiveDivision();
    }

    // /brief setup for larrabee rejection and check if 
    bool Setup(int binID, int binIndex)
    {
        BinID = binID, BinIndex = binIndex;
        B0 = static_cast<int>(verts[1].proj.y - verts[0].proj.y);
        C0 = static_cast<int>(verts[1].proj.x - verts[0].proj.x);
        B1 = static_cast<int>(verts[2].proj.y - verts[1].proj.y);
        C1 = static_cast<int>(verts[2].proj.x - verts[1].proj.x);
        B2 = static_cast<int>(verts[0].proj.y - verts[2].proj.y);
        C2 = static_cast<int>(verts[0].proj.x - verts[2].proj.x);

        const int det = C2 * -B1 + C1 * B2;

        if (det < 0)
        {
            B0 *= -1;
            C0 *= -1;
            B1 *= -1;
            C1 *= -1;
            B2 *= -1;
            C2 *= -1;
        }

        if (det == 0)
        {
            return false;
        }

        invDet = 1.0f / static_cast<float>(det);

        Vec2 edgeNormal1 = Vec2(-B0, C0);
        Vec2 edgeNormal2 = Vec2(-B1, C1);
        Vec2 edgeNoraml3 = Vec2(-B2, C2);

        // set up bounding box
        maxX = static_cast<int>(std::max<float>(std::max<float>(verts[0].proj.x, verts[1].proj.x), verts[2].proj.x));
        maxY = static_cast<int>(std::max<float>(std::max<float>(verts[0].proj.y, verts[1].proj.y), verts[2].proj.y));
        minX = static_cast<int>(std::min<float>(std::min<float>(verts[0].proj.x, verts[1].proj.x), verts[2].proj.x));
        minY = static_cast<int>(std::min<float>(std::min<float>(verts[0].proj.y, verts[1].proj.y), verts[2].proj.y));


        if (edgeNormal1.x > 0)
        {
            if (edgeNormal1.y > 0)
            {
                rejectIndex0 = 3;
                acceptIndex0 = 0;
            }
            else
            {
                rejectIndex0 = 1;
                acceptIndex0 = 2;
            }
        }
        else
        {
            if (edgeNormal1.y > 0)
            {
                rejectIndex0 = 2;
                acceptIndex0 = 1;
            }
            else
            {
                rejectIndex0 = 0;
                acceptIndex0 = 3;
            }
        }

        if (edgeNormal2.x > 0)
        {
            if (edgeNormal2.y > 0)
            {
                rejectIndex1 = 3;
                acceptIndex1 = 0;
            }
            else
            {
                rejectIndex1 = 1;
                acceptIndex1 = 2;
            }
        }
        else
        {
            if (edgeNormal2.y > 0)
            {
                rejectIndex1 = 2;
                acceptIndex1 = 1;
            }
            else
            {
                rejectIndex1 = 0;
                acceptIndex1 = 3;
            }
        }

        if (edgeNoraml3.x > 0)
        {
            if (edgeNoraml3.y > 0)
            {
                rejectIndex2 = 3;
                acceptIndex2 = 0;
            }
            else
            {
                rejectIndex2 = 1;
                acceptIndex2 = 2;
            }
        }
        else
        {
            if (edgeNoraml3.y > 0)
            {
                rejectIndex2 = 2;
                acceptIndex2 = 1;
            }
            else
            {
                rejectIndex2 = 0;
                acceptIndex2 = 3;
            }
        }

        return det > 0;
    }

    // edge function 
    float EdgeFunc0(Vec2& p)
    {
        return B0 * (p.x - verts[0].proj.x) - C0 * (p.y - verts[0].proj.y);
    }

    float EdgeFunc1(Vec2& p)
    {
        return B1 * (p.x - verts[1].proj.x) - C1 * (p.y - verts[1].proj.y);
    }

    float EdgeFunc2(Vec2& p)
    {
        return B2 * (p.x - verts[2].proj.x) - C2 * (p.y - verts[2].proj.y);
    }

    bool CheckInTriangle(Vec2& point)
    {
        float e0 = EdgeFunc0(point);
        float e1 = EdgeFunc1(point);
        float e2 = EdgeFunc2(point);
        return e0 < 0 && e1 < 0 && e2 < 0;
    }
};

constexpr float epsilon = -std::numeric_limits<float>::epsilon();


struct SIMDTriangle
{
    SIMDFloat B0{}, C0{}, B1{}, C1{}, B2{}, C2{};
    SIMDFloat invDet;
    SIMDVec3 normal1, normal2, normal3;
    SIMDVec2 v1, v2, v3;
    SIMDFloat invW1, invW2, invW3;
    SIMDVec2 tex1, tex2, tex3;

    SIMDTriangle(const Triangle& t)
        : B0(SIMDFloat(static_cast<float>(t.B0))),
          C0(SIMDFloat(static_cast<float>(t.C0))),
          B1(SIMDFloat(static_cast<float>(t.B1))),
          C1(SIMDFloat(static_cast<float>(t.C1))),
          B2(SIMDFloat(static_cast<float>(t.B2))), C2(SIMDFloat(static_cast<float>(t.C2)))
      , normal1(SIMDVec3(t.verts[0].normal.x,
                         t.verts[0].normal.y,
                         t.verts[0].normal.z))
      , normal2(SIMDVec3(t.verts[1].normal.x,
                         t.verts[1].normal.y,
                         t.verts[1].normal.z))
      , normal3(SIMDVec3(t.verts[2].normal.x,
                         t.verts[2].normal.y,
                         t.verts[2].normal.z))
      ,
          v1(SIMDVec2(t.verts[0].proj.x, t.verts[0].proj.y)),
          v2(SIMDVec2(t.verts[1].proj.x, t.verts[1].proj.y)),
          v3(SIMDVec2(t.verts[2].proj.x, t.verts[2].proj.y)),
          invW1(t.verts[0].invW),
          invW2(t.verts[1].invW),
          invW3(t.verts[2].invW),
          tex1(SIMDVec2(t.verts[0].tex.x, t.verts[0].tex.y)),
          tex2(SIMDVec2(t.verts[1].tex.x, t.verts[1].tex.y)),
          tex3(SIMDVec2(t.verts[2].tex.x, t.verts[2].tex.y)),
          invDet(t.invDet)
    {
    };

    SIMDFloat EdgeFunc0(SIMDVec2& p)
    {
        return B0 * (p.x - v1.x) - C0 * (p.y - v1.y);
    }

    SIMDFloat EdgeFunc1(SIMDVec2& p)
    {
        return B1 * (p.x - v2.x) - C1 * (p.y - v2.y);
    }

    SIMDFloat EdgeFunc2(SIMDVec2& p)
    {
        return B2 * (p.x - v3.x) - C2 * (p.y - v3.y);
    }

    SIMDFloat IsInTriangle(SIMDPixel& p)
    {
        SIMDFloat e1 = EdgeFunc0(p.position);
        SIMDFloat e2 = EdgeFunc1(p.position);
        SIMDFloat e3 = EdgeFunc2(p.position);
        return (e1 <= 0.0f & e2 <= 0.0f & e3 <= 0.0f);
    }


    void ComputeBarcentric(SIMDFloat& x, SIMDFloat& y, SIMDFloat& alpha, SIMDFloat& beta, SIMDFloat& gamma)
    {
        alpha = (B1 * -1 * (x - v3.x) + C1 * (y - v3.y)) * invDet;
        beta = (B2 * -1 * (x - v3.x) + C2 * (y - v3.y)) * invDet;
        gamma = SIMDFloat(1.0) - alpha - beta;
    }
};


struct Point
{
    Vec4 position{};
    Vec3 weights{};

    Point()
    {
    }

    Point(const Vec4& point, const Vec3& weights) : position(point), weights(weights)
    {
    }
};


struct MeshInstance
{
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};


struct Mesh
{
    std::vector<Triangle> tris;
    // flag to tell if we should load normal or generate them automatically while rendering
    bool hasNormal;
    bool hasUV;

    Mesh() : hasNormal(false), hasUV(false)
    {
    };

    // test methods
    Mesh(MeshInstance& m) : hasNormal(false), hasUV(false)
    {
        int triangleCount = m.indices.size() / 3;

        for (int i = 0; i < triangleCount; i++)
        {
            auto v1 = m.vertices[m.indices[3 * i]];
            auto v2 = m.vertices[m.indices[3 * i + 1]];
            auto v3 = m.vertices[m.indices[3 * i + 2]];
            tris.push_back(Triangle(v1, v2, v3));
        }
    }
};
