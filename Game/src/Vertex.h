//---------------------------------------------------------------------------------
// Vertex.h
//---------------------------------------------------------------------------------
// 
// A Vertex loaded in an obj file
//
#pragma once

#include <iostream>

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

    // used as a hashing function for when we're loading obj files
    std::string ToString()
    {
        return "{" + std::to_string(tex_id) + 
                     localPosition.ToString() + 
                     localNormal.ToString() + 
                     uv.ToString() + "}" ;
    }

    void PerspectiveDivision()
    {
        invW = 1 / proj.W;
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
