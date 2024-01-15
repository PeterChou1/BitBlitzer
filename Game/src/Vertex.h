#pragma once

#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Assets.h"

struct Vertex
{
    // Shader ID of this vertex
    ShaderAsset shader_id{};
    // texture ID of the texture that shade this vertex
    int tex_id{};
    Vec3 color{};
    // texture uv
    Vec2 uv{};
    // local data
    Vec3 localPosition;
    Vec3 localNormal;
    // world space data
    Vec3 pos{};
    // vertex normal
    Vec3 normal{};
    // 4d homogenous coordinates output by vertex shader 
    Vec4 proj{};
    // inverse w used for perspective corrected interpolation
    float invW{};

    Vertex()
    {
    }

    Vertex(Vec3& pos) : pos(pos)
    {
    }

    Vertex(Vec3& pos, Vec2& tex) : pos(pos), uv(tex)
    {
    }

    Vertex(Vec3& pos, Vec3& normal, Vec2& tex) : uv(tex), pos(pos), normal(normal)
    {
    }

    void PerspectiveDivision()
    {
        invW = 1 / proj.w;
        proj *= invW;
        uv *= invW;
    }

    Vertex operator*(float t) const
    {
        auto copy = *this;
        copy.pos *= t;
        copy.uv *= t;
        copy.normal *= t;
        return copy;
    }


    Vertex operator+(const Vertex& v) const
    {
        auto copy = *this;

        copy.pos += v.pos;
        copy.uv += v.uv;
        copy.normal += v.normal;

        return copy;
    }


    bool operator==(const Vertex& rhs) const
    {
        return uv == rhs.uv && normal == rhs.normal &&
            pos == rhs.pos && invW == rhs.invW && tex_id == rhs.tex_id;
    }
};

// hash method used: https://en.cppreference.com/w/Talk:cpp/utility/hash
template <>
struct std::hash<Vertex>
{
    std::size_t operator()(Vertex const& v) const noexcept
    {
        std::size_t h1 = std::hash<size_t>{}(v.tex_id);
        h1 = h1 ^ (std::hash<float>{}(v.uv.x) << 1);
        h1 = h1 ^ (std::hash<float>{}(v.uv.y) << 2);
        h1 = h1 ^ (std::hash<float>{}(v.localPosition.x) << 3);
        h1 = h1 ^ (std::hash<float>{}(v.localPosition.y) << 4);
        h1 = h1 ^ (std::hash<float>{}(v.localPosition.z) << 5);
        h1 = h1 ^ (std::hash<float>{}(v.localNormal.x) << 6);
        h1 = h1 ^ (std::hash<float>{}(v.localNormal.y) << 7);
        return h1;
    }
};