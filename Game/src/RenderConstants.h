#pragma once
#include <numeric>
#include <unordered_map>
#include <thread>

#include "Assets.h"
#include "Entity.h"
#include "Resource.h"

using BufferRange = std::pair<int, int>;


/**
 * \brief Render Constants is responsible to keeping track with
 *        constants related to the rendering process
 */
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
    
    std::unordered_map<Entity, ShaderAsset> EntityToShaderAsset;
    // Maps which entity has which index/vertex ranges in the index and
    // vertex buffer
    std::unordered_map<Entity, BufferRange> EntityToVertexRange;
    std::unordered_map<Entity, BufferRange> EntityToIndexRange;

    std::vector<std::uint32_t> CoreIds;
    int CoreInterval{};
    unsigned int CoreCount{};
    std::uint32_t TriangleCount{};
};