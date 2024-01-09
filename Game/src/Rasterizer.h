#pragma once
#include "ClippedTriangleBuffer.h"
#include "PixelBuffer.h"
#include "RenderConstants.h"
#include "Tiles.h"


class Rasterizer
{
public:
    Rasterizer();

    void Rasterize();

private:

    void AssignTile();

    void RasterizeTiles();

    std::shared_ptr<ClippedTriangleBuffer> m_ClippedTriangle;
    std::shared_ptr<Tiles> m_Tiles;
    std::shared_ptr<PixelBuffer> m_PixelBuffer;
    std::shared_ptr<DepthBuffer> m_DepthBuffer;
    std::shared_ptr<RenderConstants> m_RenderConstants;
};
