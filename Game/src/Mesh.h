#pragma once
#include "Assets.h"

struct Mesh
{
    bool Loaded{};
    bool MarkedForDeletion{};
    ObjAsset MeshType;
    Mesh() = default;
    Mesh(ObjAsset meshType) : MeshType(meshType) {}
};
