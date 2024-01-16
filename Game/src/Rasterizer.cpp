#include "stdafx.h"

#include <cmath>
#include "Rasterizer.h"

#include "Concurrent.h"
#include "ECSManager.h"
#include "SIMDTriangle.h"


extern ECSManager ECS;


Rasterizer::Rasterizer()
{
    m_ClippedTriangle = ECS.GetResource<ClippedTriangleBuffer>();
    m_Tiles = ECS.GetResource<Tiles>();
    m_PixelBuffer = ECS.GetResource<PixelBuffer>();
    m_DepthBuffer = ECS.GetResource<DepthBuffer>();
    m_RenderConstants = ECS.GetResource<RenderConstants>();
}

void Rasterizer::Rasterize()
{
    AssignTile();
    RasterizeTiles();
    m_PixelBuffer->AccumulatePixel(*m_DepthBuffer);
}

void Rasterizer::AssignTile()
{
    std::vector<unsigned int> coreID = m_RenderConstants->CoreIds;
    std::vector<Tile>& tiles = m_Tiles->TilesArray;

    Concurrent::ForEach(coreID.begin(), coreID.end(), [&](unsigned int binID)
    {
            std::vector<Triangle>& binnedTriangles = m_ClippedTriangle->Buffer[binID];

            for (auto& tri : binnedTriangles)
            {
                // iterate through triangle bounding box

                const int startX = tri.minX / TILE_SIZE_X;
                const int endX = (tri.maxX + TILE_SIZE_X - 1) / TILE_SIZE_X;
                const int startY = tri.minY / TILE_SIZE_Y;
                const int endY = (tri.maxY + TILE_SIZE_Y - 1) / TILE_SIZE_Y;

                for (int x = startX; x < endX; x++)
                {
                    for (int y = startY; y < endY; y++)
                    {
                        const int index = y * TILE_COUNT_X + x;

                        if (index >= tiles.size())
                            continue;

                        Tile& t = tiles[index];

                        Vec2 corner0 = t.GetCorner(tri.rejectIndex0);
                        Vec2 corner1 = t.GetCorner(tri.rejectIndex1);
                        Vec2 corner2 = t.GetCorner(tri.rejectIndex2);

                        const float e0 = tri.EdgeFunc0(corner0);
                        const float e1 = tri.EdgeFunc1(corner1);
                        const float e2 = tri.EdgeFunc2(corner2);

                        // trivial reject
                        if (e0 > 0 || e1 > 0 || e2 > 0)
                        {
                            continue;
                        }
                        t.Add(binID, tri);
                    }
                }
            }
     });
}

void Rasterizer::RasterizeTiles()
{
    std::vector<Tile>& tiles = m_Tiles->TilesArray;

    Concurrent::ForEach(tiles.begin(), tiles.end(), [&] (Tile& tile)
    {
        std::vector<std::vector<Triangle>>& binTriangles = tile.GetBinTriangle();

        Vec2 tileMin = tile.GetMin();
        Vec2 tileMax = tile.GetMax();

        for (auto& binTriangle : binTriangles)
        {
            for (auto& tri : binTriangle)
            {
                // construct the triangles bounding box to loop through this faster
                auto minPt =
                    Vec2((std::min)((std::max)(tileMin.X, static_cast<float>(tri.minX)),
                        static_cast<float>(tri.maxX)),
                        (std::min)((std::max)(tileMin.Y, static_cast<float>(tri.minY)),
                            static_cast<float>(tri.maxY)));

                auto maxPt =
                    Vec2((std::max)((std::min)(tileMax.X, static_cast<float>(tri.maxX)),
                        static_cast<float>(tri.minX)),
                        (std::max)((std::min)(tileMax.Y, static_cast<float>(tri.maxY)),
                            static_cast<float>(tri.minY)));

                // iterate over pixels to determine which pixel belong in the triangle

                // Aligning pixel tile boundaries
                minPt.X =
                    std::floor(minPt.X / SIMDPixel::PIXEL_WIDTH) * SIMDPixel::PIXEL_WIDTH;
                minPt.Y =
                    std::floor(minPt.Y / SIMDPixel::PIXEL_HEIGHT) * SIMDPixel::PIXEL_HEIGHT;
                maxPt.X =
                    std::ceil(maxPt.X / SIMDPixel::PIXEL_WIDTH) * SIMDPixel::PIXEL_WIDTH;
                maxPt.Y =
                    std::ceil(maxPt.Y / SIMDPixel::PIXEL_HEIGHT) * SIMDPixel::PIXEL_HEIGHT;

                SIMDTriangle triSIMD(tri);

                SIMDFloat posX = SIMDPixel::PixelOffsetX + minPt.X;
                SIMDFloat posY = SIMDPixel::PixelOffsetY + minPt.Y;

                SIMDFloat deltaX0 = triSIMD.B0 * SIMDPixel::PIXEL_WIDTH;
                SIMDFloat deltaX1 = triSIMD.B1 * SIMDPixel::PIXEL_WIDTH;
                SIMDFloat deltaX2 = triSIMD.B2 * SIMDPixel::PIXEL_WIDTH;
                SIMDFloat deltaY0 = triSIMD.C0 * SIMDPixel::PIXEL_HEIGHT;
                SIMDFloat deltaY1 = triSIMD.C1 * SIMDPixel::PIXEL_HEIGHT;
                SIMDFloat deltaY2 = triSIMD.C2 * SIMDPixel::PIXEL_HEIGHT;

                SIMDVec2 base = SIMDVec2(posX, posY);
                SIMDFloat e1 = triSIMD.EdgeFunc0(base);
                SIMDFloat e2 = triSIMD.EdgeFunc1(base);
                SIMDFloat e3 = triSIMD.EdgeFunc2(base);

                for (int y = minPt.Y; y < maxPt.Y; y += SIMDPixel::PIXEL_HEIGHT)
                {
                    SIMDFloat deltaXe1 = e1;
                    SIMDFloat deltaXe2 = e2;
                    SIMDFloat deltaXe3 = e3;
                    posX = SIMDPixel::PixelOffsetX + minPt.X;
                    for (int x = minPt.X; x < maxPt.X; x += SIMDPixel::PIXEL_WIDTH)
                    {
                        SIMDFloat inTriangle =
                            deltaXe1 <= 0.0f & deltaXe2 <= 0.0f & deltaXe3 <= 0.0f; // &
                        //(deltaXe1 <= epsilon | deltaXe2 <= epsilon | deltaXe3 <= epsilon);


                        if (SIMD::Any(inTriangle))
                        {
                            SIMDFloat alpha, beta, gamma;
                            triSIMD.ComputeBarycentric(posX, posY, alpha, beta, gamma);
                            SIMDFloat depth = alpha * triSIMD.InvW1 + beta * triSIMD.InvW2 +
                                gamma * triSIMD.InvW3;

                            int pixelX = x / SIMDPixel::PIXEL_WIDTH;
                            int pixelY = y / SIMDPixel::PIXEL_HEIGHT;
                            SIMDFloat visible = m_DepthBuffer->DepthTest(pixelX, pixelY, depth, inTriangle);

                            if (SIMD::Any(visible))
                            {
                                m_DepthBuffer->UpdateBuffer(pixelX, pixelY, visible, depth);
                                SIMDPixel pixel = SIMDPixel(SIMDVec2(posX, posY), depth, alpha, beta, gamma, tri.BinID,
                                    tri.BinIndex);
                                // We deferred the shading to the fragment shading stage
                                m_PixelBuffer->SetBuffer(pixelX, pixelY, pixel, visible);
                            }
                        }
                        deltaXe1 = deltaXe1 + deltaX0;
                        deltaXe2 = deltaXe2 + deltaX1;
                        deltaXe3 = deltaXe3 + deltaX2;
                        posX = posX + static_cast<float>(SIMDPixel::PIXEL_WIDTH);
                    }
                    e1 = e1 - deltaY0;
                    e2 = e2 - deltaY1;
                    e3 = e3 - deltaY2;
                    posY = posY + static_cast<float>(SIMDPixel::PIXEL_HEIGHT);
                }
            }
        }

    });

}


