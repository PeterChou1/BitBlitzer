#include "stdafx.h"
#include <thread>
#include <iostream>
#include <algorithm>
#include "RendererM.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "SIMD.h"
#include "../App/app.h"


extern Coordinator gCoordinator;

RendererM::RendererM(GraphicsBuffer& g, Camera& cam, ColorBuffer& color, DepthBufferSIMD& depth, TextureList& textureList) :
    m_vertexBuffer(g.GetVertexBuffer()), m_indexBuffer(g.GetIndexBuffer()),
    m_projectedVertexBuffer(g.GetProjectedVertex()), m_projectedClip(g.GetProjectedClipped()), m_tiles(g.GetTiles())
  , m_cam(cam)
  , m_color(color)
  , m_depth(depth)
  , m_textureList(textureList)
{
    m_coreCount = std::thread::hardware_concurrency();
    m_coreInterval = (g.GetTriangleCount() + m_coreCount - 1) / m_coreCount;
    m_coreIds.resize(m_coreCount);
    std::iota(m_coreIds.begin(), m_coreIds.end(), 0);
}

void RendererM::Render()
{
    VertexShaderStage();
    ClippingStage();
    TiledRasterizationStage();
    RasterizationStage();
    FragmentShaderStage();
    UpdateFrameBuffer();
    ClearBuffer();
}

void RendererM::VertexShaderStage()
{
    Concurrency::ForEach(m_vertexBuffer.begin(), m_vertexBuffer.end(), [&](Vertex& v)
    {
        //Transform modelTransform = gCoordinator.GetComponent<Transform>(v.id);
        Vertex projected = v;
        //projected.pos = modelTransform.TransformVec3(projected.pos);
        //projected.normal = modelTransform.TransformNormal(projected.normal);
        projected.proj = m_cam.proj * Vec4(m_cam.WorldToCamera(projected.pos));
        m_projectedVertexBuffer[v.index] = projected;
    });
}

void RendererM::ClippingStage()
{
    // std::cout << "clipping start" << std::endl;
    Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin)
    {
        int start = bin * m_coreInterval;
        int end = (bin + 1) * m_coreInterval;

        std::vector<Triangle>& binProjectedClip = m_projectedClip[bin];

        for (int i = start; i < end; i++)
        {
            if (3 * i + 2 > m_indexBuffer.size()) break;

            assert(m_indexBuffer[3 * i] < m_projectedVertexBuffer.size());
            assert(m_indexBuffer[3 * i + 1] < m_projectedVertexBuffer.size());
            assert(m_indexBuffer[3 * i + 2] < m_projectedVertexBuffer.size());

            Vertex v1 = m_projectedVertexBuffer[m_indexBuffer[3 * i]];
            Vertex v2 = m_projectedVertexBuffer[m_indexBuffer[3 * i + 1]];
            Vertex v3 = m_projectedVertexBuffer[m_indexBuffer[3 * i + 2]];
            // back face culling
            auto t = Triangle(v1, v2, v3);

            if (v1.normal.Dot(v1.pos - m_cam.pos) < 0)
            {
                std::vector<Triangle> clipped = Clip(t);
                // Output projected position to raster space
                for (Triangle& clip : clipped)
                {
                    m_cam.ToRasterSpace(clip.verts[0].proj);
                    m_cam.ToRasterSpace(clip.verts[1].proj);
                    m_cam.ToRasterSpace(clip.verts[2].proj);
                    if (clip.Setup())
                    {
                        binProjectedClip.push_back(clip);
                    }
                }
            }
        }
    });
}

void RendererM::TiledRasterizationStage()
{
    Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin)
    {
        std::vector<Triangle>& binProjectedClip = m_projectedClip[bin];
        for (auto& tri : binProjectedClip)
        {
            // iterate through triangle bounding box

            const int startX = tri.minX / TileSizeX;
            const int endX = (tri.maxX + TileSizeX - 1) / TileSizeX;
            const int startY = tri.minY / TileSizeY;
            const int endY = (tri.maxY + TileSizeY - 1) / TileSizeY;


            for (int x = startX; x < endX; x++)
            {
                for (int y = startY; y < endY; y++)
                {
                    const int index = y * TileCountX + x;

                    if (index >= m_tiles.size()) continue;

                    Tile& t = m_tiles[index];

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
                    t.Add(bin, tri);
                }
            }
        }
    });
}


void RendererM::RasterizationStage()
{
    Concurrency::ForEach(m_tiles.begin(), m_tiles.end(), [&](Tile& tile)
    {
        std::vector<std::vector<Triangle>>& binTriangles = tile.GetBinTriangle();

        for (auto& binTriangle : binTriangles)
        {
            for (auto& tri : binTriangle)
            {
                Vec2 tileMin = tile.GetMin();
                Vec2 tileMax = tile.GetMax();
                auto minPt = Vec2(
                    (std::min)((std::max)(tileMin.x, static_cast<float>(tri.minX)), static_cast<float>(tri.maxX)),
                    (std::min)((std::max)(tileMin.y, static_cast<float>(tri.minY)), static_cast<float>(tri.maxY))
                );

                auto maxPt = Vec2(
                    (std::max)((std::min)(tileMax.x, static_cast<float>(tri.maxX)), static_cast<float>(tri.minX)),
                    (std::max)((std::min)(tileMax.y, static_cast<float>(tri.maxY)), static_cast<float>(tri.minY))
                );

                SimpleTexture texture = m_textureList.textureList[tri.verts[0].tex_id];
                // iterate over pixels to determine which pixel belong in the triangle

                // Aligning pixel tile boundaries
                minPt.x = std::floor(minPt.x / SIMDPixel::pixelWidth) * SIMDPixel::pixelWidth;
                minPt.y = std::floor(minPt.y / SIMDPixel::pixelHeight) * SIMDPixel::pixelHeight;
                maxPt.x = std::ceil(maxPt.x / SIMDPixel::pixelWidth) * SIMDPixel::pixelWidth;
                maxPt.y = std::ceil(maxPt.y / SIMDPixel::pixelHeight) * SIMDPixel::pixelHeight;

                SIMDTriangle triSIMD(tri);

                auto basePixel = SIMDPixel(minPt.x, minPt.y);

                SIMDFloat deltaX0 = triSIMD.B0 * SIMDPixel::pixelWidth;
                SIMDFloat deltaX1 = triSIMD.B1 * SIMDPixel::pixelWidth;
                SIMDFloat deltaX2 = triSIMD.B2 * SIMDPixel::pixelWidth;
                SIMDFloat deltaY0 = triSIMD.C0 * SIMDPixel::pixelHeight;
                SIMDFloat deltaY1 = triSIMD.C1 * SIMDPixel::pixelHeight;
                SIMDFloat deltaY2 = triSIMD.C2 * SIMDPixel::pixelHeight;

                SIMDFloat e1 = triSIMD.EdgeFunc0(basePixel.position);
                SIMDFloat e2 = triSIMD.EdgeFunc1(basePixel.position);
                SIMDFloat e3 = triSIMD.EdgeFunc2(basePixel.position);

                for (int pixelY = minPt.y; pixelY < maxPt.y; pixelY += SIMDPixel::pixelHeight)
                {
                    SIMDFloat baseXe1 = e1;
                    SIMDFloat baseXe2 = e2;
                    SIMDFloat baseXe3 = e3;
                    SIMDPixel pixel = basePixel;

                    for (int pixelX = minPt.x; pixelX < maxPt.x; pixelX += SIMDPixel::pixelWidth)
                    {
                        SIMDFloat inTriangle = baseXe1 <= 0.0f & baseXe2 <= 0.0f & baseXe3 <= 0.0f & 
                            (baseXe1 <= epsilon| baseXe2 <= epsilon | baseXe3 <= epsilon);


                        if (SIMD::Any(inTriangle))
                        {
                            SIMDFloat alpha, beta, gamma;
                            triSIMD.ComputeBarcentric(pixel, alpha, beta, gamma);
                            SIMDFloat depth = alpha * triSIMD.invW1 + beta * triSIMD.invW2 + gamma * triSIMD.invW3;
                            SIMDFloat test = m_depth.DepthTest(pixelX, pixelY, depth, inTriangle);

                            if (SIMD::Any(test))
                            {
                                pixel.mask = test;
                                SIMDVec2 texUV = (triSIMD.tex1 * alpha + triSIMD.tex2 * beta + triSIMD.tex3 * gamma) / depth;
                                float r, g, b;
                                if (pixel.mask.GetBit(0)) 
                                {
                                    texture.Sample(texUV.x[0], texUV.y[0], r, g, b);
                                    m_color.SetColor(pixel.position.x[0], pixel.position.y[0], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(1)) {
                                    texture.Sample(texUV.x[1], texUV.y[1], r, g, b);
                                    m_color.SetColor(pixel.position.x[1], pixel.position.y[1], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(2)) {
                                    texture.Sample(texUV.x[2], texUV.y[2], r, g, b);
                                    m_color.SetColor(pixel.position.x[2], pixel.position.y[2], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(3)) {
                                    texture.Sample(texUV.x[3], texUV.y[3], r, g, b);
                                    m_color.SetColor(pixel.position.x[3], pixel.position.y[3], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(4)) {
                                    texture.Sample(texUV.x[4], texUV.y[4], r, g, b);
                                    m_color.SetColor(pixel.position.x[4], pixel.position.y[4], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(5)) {
                                    texture.Sample(texUV.x[5], texUV.y[5], r, g, b);
                                    m_color.SetColor(pixel.position.x[5], pixel.position.y[5], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(6)) {
                                    texture.Sample(texUV.x[6], texUV.y[6], r, g, b);
                                    m_color.SetColor(pixel.position.x[6], pixel.position.y[6], r, g, b);
                                }
                                
                                if (pixel.mask.GetBit(7)) {
                                    texture.Sample(texUV.x[7], texUV.y[7], r, g, b);
                                    m_color.SetColor(pixel.position.x[7], pixel.position.y[7], r, g, b);
                                }
                            }
                        }
                        baseXe1 = baseXe1 + deltaX0;
                        baseXe2 = baseXe2 + deltaX1;
                        baseXe3 = baseXe3 + deltaX2;
                        pixel.position.x =
                          pixel.position.x + SIMDPixel::pixelWidth;
                    }

                    e1 = e1 - deltaY0;
                    e2 = e2 - deltaY1;
                    e3 = e3 - deltaY2;
                    basePixel.position.y =
                      basePixel.position.y + SIMDPixel::pixelHeight;
                }
            }
        }
    });
    // std::cout << maxDepth;
    App::RenderTexture(m_color.GetBuffer());
}

void RendererM::FragmentShaderStage()
{

}

void RendererM::UpdateFrameBuffer()
{
}

void RendererM::ClearBuffer()
{
    m_depth.ClearBuffer();
    m_color.ClearBuffer();
    Concurrency::ForEach(m_tiles.begin(), m_tiles.end(), [&](Tile& tile)
    {
        tile.Clear();
    });
    Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin)
    {
        m_projectedClip[bin].clear();
    });
}
