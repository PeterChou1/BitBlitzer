//---------------------------------------------------------------------------------
// VertexBuffer.h
//---------------------------------------------------------------------------------
//
// The VertexBuffer stores all vertices in the game scene that make up
// the loaded meshes
//
#pragma once
#include <vector>

#include "Resource.h"
#include "Vertex.h"


class VertexBuffer : public Resource
{
public:

    void ResetResource() override
    {
        Buffer.clear();
    }

    std::vector<Vertex> Buffer;
};
