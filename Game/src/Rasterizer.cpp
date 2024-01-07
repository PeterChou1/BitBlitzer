#include "stdafx.h"

#include <cmath>
#include "Rasterizer.h"
#include "SIMDTriangle.h"


void Rendering::AssignTile(int binID,
    std::vector<std::vector<Triangle>>& ProjectedClip,
    std::vector<Tile>& Tiles)
{
    std::vector<Triangle>& binnedTriangles = ProjectedClip[binID];

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

                if (index >= Tiles.size())
                    continue;

                Tile& t = Tiles[index];

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
}

void Rendering::RasterizeTile(
    Tile& tile,
    SIMDPixelBuffer& pixelBuffer,
    SIMDDepthBuffer& depthBuffer)
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
                Vec2((std::min)((std::max)(tileMin.x, static_cast<float>(tri.minX)),
                    static_cast<float>(tri.maxX)),
                    (std::min)((std::max)(tileMin.y, static_cast<float>(tri.minY)),
                        static_cast<float>(tri.maxY)));

            auto maxPt =
                Vec2((std::max)((std::min)(tileMax.x, static_cast<float>(tri.maxX)),
                    static_cast<float>(tri.minX)),
                    (std::max)((std::min)(tileMax.y, static_cast<float>(tri.maxY)),
                        static_cast<float>(tri.minY)));

            // iterate over pixels to determine which pixel belong in the triangle

            // Aligning pixel tile boundaries
            minPt.x =
                std::floor(minPt.x / SIMDPixel::PIXEL_WIDTH) * SIMDPixel::PIXEL_WIDTH;
            minPt.y =
                std::floor(minPt.y / SIMDPixel::PIXEL_HEIGHT) * SIMDPixel::PIXEL_HEIGHT;
            maxPt.x =
                std::ceil(maxPt.x / SIMDPixel::PIXEL_WIDTH) * SIMDPixel::PIXEL_WIDTH;
            maxPt.y =
                std::ceil(maxPt.y / SIMDPixel::PIXEL_HEIGHT) * SIMDPixel::PIXEL_HEIGHT;

            SIMDTriangle triSIMD(tri);

            SIMDFloat posX = SIMDPixel::PixelOffsetX + minPt.x;
            SIMDFloat posY = SIMDPixel::PixelOffsetY + minPt.y;

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

            for (int y = minPt.y; y < maxPt.y; y += SIMDPixel::PIXEL_HEIGHT)
            {
                SIMDFloat deltaXe1 = e1;
                SIMDFloat deltaXe2 = e2;
                SIMDFloat deltaXe3 = e3;
                posX = SIMDPixel::PixelOffsetX + minPt.x;
                for (int x = minPt.x; x < maxPt.x; x += SIMDPixel::PIXEL_WIDTH)
                {
                    SIMDFloat inTriangle =
                        deltaXe1 <= 0.0f & deltaXe2 <= 0.0f & deltaXe3 <= 0.0f; // &
                    //(deltaXe1 <= epsilon | deltaXe2 <= epsilon | deltaXe3 <= epsilon);


                    if (SIMD::Any(inTriangle))
                    {
                        SIMDFloat alpha, beta, gamma;
                        triSIMD.ComputeBarycentric(posX, posY, alpha, beta, gamma);
                        SIMDFloat depth = alpha * triSIMD.invW1 + beta * triSIMD.invW2 +
                            gamma * triSIMD.invW3;

                        int pixelX = x / SIMDPixel::PIXEL_WIDTH;
                        int pixelY = y / SIMDPixel::PIXEL_HEIGHT;
                        SIMDFloat visible = depthBuffer.DepthTest(pixelX, pixelY, depth, inTriangle);

                        if (SIMD::Any(visible))
                        {
                            depthBuffer.UpdateBuffer(pixelX, pixelY, visible, depth);
                            SIMDPixel pixel = SIMDPixel(SIMDVec2(posX, posY), depth, alpha, beta, gamma, tri.BinID,
                                tri.BinIndex);
                            // We deferred the shading to the fragment shading stage
                            pixelBuffer.SetBuffer(pixelX, pixelY, pixel, visible);
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
}
