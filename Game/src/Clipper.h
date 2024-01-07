#pragma once
#include <cstdint>
#include <vector>

#include "Camera.h"
#include "Triangle.h"


namespace Rendering
{
    void Clip(
        Camera& cam,
        std::vector<std::vector<Triangle>>& projectedClippedTriangle,
        std::vector<Vertex>& projectedVertexBuffer,
        std::vector<std::uint32_t>& indexBuffer,
        int binID, int start, int end
    );
}

