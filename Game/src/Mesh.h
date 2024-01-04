#pragma once
#include <vector>
#include "Entity.h"
#include "Assets.h"
#include "Vertex.h"
#include "Transform.h"

struct MeshInstance
{
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    void transform(Transform& t)
    {
        for (Vertex& v : vertices)
        {
            v.pos = t.TransformVec3(v.localPosition);
            v.normal = t.TransformNormal(v.localNormal);
        }
    }
};


struct Mesh
{
    bool loaded{};
    bool markedForDeletion{};
    ObjAsset MeshType;
    Mesh() = default;
    Mesh(ObjAsset meshType) : MeshType(meshType) {}
};
