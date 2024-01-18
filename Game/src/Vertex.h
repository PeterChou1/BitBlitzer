//---------------------------------------------------------------------------------
// Vertex.h
//---------------------------------------------------------------------------------
// 
// A Vertex loaded from an obj file contains normals, position, UV coordinates
//
#pragma once

#include <iostream>

#include "Vec3.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Assets.h"

enum CubeMapFaces
{
    None,
    Back,
    Bottom,
    Front,
    Left,
    Right,
    Top
};

struct Vertex
{
    CubeMapFaces cubeMapID = None;
    // Shader ID of this vertex
    ShaderAsset ShaderID{};
    // texture ID of the texture that shade this vertex
    int TextureID{};
    Vec3 Color{};
    // texture uv
    Vec2 UV{};
    // vertex in local space
    Vec3 LocalPosition;
    Vec3 LocalNormal;
    // vertex in world space
    Vec3 Position{};
    Vec3 Normal{};
    // 4d homogenous coordinates output by vertex shader 
    Vec4 Projection{};
    // inverse w used for perspective corrected interpolation
    float InverseW{};

    Vertex() = default;

    Vertex(const Vec3& pos) : Position(pos)
    {
    }

    Vertex(const Vec3& pos, const Vec2& tex) : UV(tex), Position(pos)
    {
    }

    Vertex(const Vec3& pos, const Vec3& normal, const Vec2& tex) : UV(tex), Position(pos), Normal(normal)
    {
    }

    // used as a hashing function for when we're loading obj files
    std::string ToString()
    {
        return "{" + std::to_string(TextureID) + 
                     LocalPosition.ToString() + 
                     LocalNormal.ToString() + 
                     UV.ToString() + "}" ;
    }

    void PerspectiveDivision()
    {
        InverseW = 1 / Projection.W;
        Projection *= InverseW;
        UV *= InverseW;
    }

    Vertex operator*(const float t) const
    {
        auto copy = *this;
        copy.Position *= t;
        copy.UV *= t;
        copy.Normal *= t;
        return copy;
    }


    Vertex operator+(const Vertex& v) const
    {
        auto copy = *this;

        copy.Position += v.Position;
        copy.UV += v.UV;
        copy.Normal += v.Normal;

        return copy;
    }


    bool operator==(const Vertex& rhs) const
    {
        return UV == rhs.UV && Normal == rhs.Normal &&
            Position == rhs.Position && InverseW == rhs.InverseW && TextureID == rhs.TextureID;
    }
};
