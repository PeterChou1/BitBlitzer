#pragma once
#include <vector>

#include "SIMDPixelBuffer.h"
#include "Tile.h"
#include "Triangle.h"



namespace Rendering
{

    void AssignTile(
        int binID, 
        std::vector<std::vector<Triangle>>& ProjectedClip,
        std::vector<Tile>& Tiles
    );

    void RasterizeTile(
        Tile& tile,
        SIMDPixelBuffer& pixelBuffer,
        SIMDDepthBuffer& depthBuffer
    );
}
