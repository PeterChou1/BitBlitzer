#pragma once
#include "Assets.h"

class Mesh
{
public:
    bool loaded{};
    bool markedForDeletion{};
    ObjAsset MeshType;
    Mesh() = default;
    Mesh(ObjAsset meshType) : MeshType(meshType) {}
};
