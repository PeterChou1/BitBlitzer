//---------------------------------------------------------------------------------
// RenderConstants.h
//---------------------------------------------------------------------------------
//
// Render Constants is responsible to keeping track with
// constants related to the rendering process
//
#pragma once
#include <numeric>
#include <unordered_map>
#include <thread>

#include "Assets.h"
#include "Entity.h"
#include "Resource.h"
#include "Vertex.h"

using BufferRange = std::pair<int, int>;

class RenderConstants : public Resource
{
public:

    RenderConstants()
    {
        CoreCount = std::thread::hardware_concurrency();
        CoreIds.resize(CoreCount);
        std::iota(CoreIds.begin(), CoreIds.end(), 0);
    }

    void ResetResource() override
    {
        EntityToIndexRange.clear();
        EntityToVertexRange.clear();
        EntityToIndexRange.clear();
        CoreInterval = 0;
        TriangleCount = 0;
    }

    // Maps which mesh entity has which shaders attach to it
    std::unordered_map<Entity, ShaderAsset> EntityToShaderAsset;
    // Maps which entity has which index/vertex ranges in the index and
    // vertex buffer
    std::unordered_map<Entity, BufferRange> EntityToVertexRange;
    std::unordered_map<Entity, BufferRange> EntityToIndexRange;
    BufferRange CubemapeRange;
    // Gives each core on the system an ID
    std::vector<std::uint32_t> CoreIds;
    // Used by clipper during multithreading clipping
    int CoreInterval{};
    unsigned int CoreCount{};
    // total triangle count of Triangle in the System
    std::uint32_t TriangleCount{};
};