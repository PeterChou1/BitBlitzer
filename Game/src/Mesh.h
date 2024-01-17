//---------------------------------------------------------------------------------
// Mesh.h
//---------------------------------------------------------------------------------
// 
// Provides A Simple Wrapper For Users To Load Meshes Through the Rendering System
//

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
