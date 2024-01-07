#include "stdafx.h"

#include <algorithm>
#include <thread>
#include <numeric>

#include "RendererAVX.h"
#include "../App/app.h"
#include "Clipper.h"
#include "Concurrent.h"
#include "SIMD.h"
#include "Camera.h"
#include "AssetServer.h"
#include "BlingPhong.h"
#include "SIMDTriangle.h"

constexpr float epsilon = -std::numeric_limits<float>::epsilon();


RendererAVX::RendererAVX(const int width, const int height, Camera& cam) : Renderer(width, height, cam)
{
    
    m_DepthBuffer = SIMDDepthBuffer(width, height);
    m_PixelBuffer = SIMDPixelBuffer(width, height);
    m_CoreCount = std::thread::hardware_concurrency();
    m_CoreInterval = 0;
    m_CoreIds.resize(m_CoreCount);
    m_ProjectedClip.resize(m_CoreCount);
    std::iota(m_CoreIds.begin(), m_CoreIds.end(), 0);
    for (int y = 0; y < TileCountY; ++y)
    {
        for (int x = 0; x < TileCountX; ++x)
        {
            auto max = Vec2((std::min)((x + 1) * TileSizeX, width), (std::min)((y + 1) * TileSizeY, height));
            auto min = Vec2((std::min)(x * TileSizeX, width), (std::min)(y * TileSizeY, height));
            auto t = Tile(min, max);
            m_Tiles.push_back(t);
        }
    }

    PointLight light;
    light.color = {1, 1, 1};
    light.lightPower = 40.0;
    light.position = {0.0, 5.0, 5.0};

    m_Lights.push_back(light);
}

void
RendererAVX::Render()
{
    VertexShaderStage();
    ClippingStage();
    TiledRasterizationStage();
    RasterizationStage();
    FragmentShaderStage();
    UpdateFrameBuffer();
    ClearBuffer();
}


void
RendererAVX::VertexShaderStage()
{
    Concurrent::ForEach(m_VertexBuffer.begin(), m_VertexBuffer.end(), [&](Vertex& v)
    {
        Vertex projected = v;
        projected.proj = m_cam.proj * Vec4(m_cam.WorldToCamera(projected.pos));
        m_ProjectedVertexBuffer[v.index] = projected;
    });
}

void
RendererAVX::ClippingStage()
{
    m_CoreInterval = (m_TriangleCount + m_CoreCount - 1) / m_CoreCount;

    Concurrent::ForEach(m_CoreIds.begin(), m_CoreIds.end(), [&](int binID)
    {
        int start = binID * m_CoreInterval;
        int end = (binID + 1) * m_CoreInterval;

        std::vector<Triangle>& binProjectedClip = m_ProjectedClip[binID];

        for (int i = start; i < end; i++)
        {
            if (3 * i + 2 > m_IndexBuffer.size())
                break;

            assert(m_IndexBuffer[3 * i] < m_ProjectedVertexBuffer.size());
            assert(m_IndexBuffer[3 * i + 1] < m_ProjectedVertexBuffer.size());
            assert(m_IndexBuffer[3 * i + 2] < m_ProjectedVertexBuffer.size());

            Vertex v1 = m_ProjectedVertexBuffer[m_IndexBuffer[3 * i]];
            Vertex v2 = m_ProjectedVertexBuffer[m_IndexBuffer[3 * i + 1]];
            Vertex v3 = m_ProjectedVertexBuffer[m_IndexBuffer[3 * i + 2]];
            // back face culling
            auto t = Triangle(v1, v2, v3);

            Vec3 normal = (v1.normal + v2.normal + v3.normal) / 3;
            //Triangle clip = t;
            if (normal.Dot(v1.pos - m_cam.pos) < epsilon)
            {
                 std::vector<Triangle> clipped = Clip(t);
                // Output projected screenSpacePosition to raster space
                for (Triangle& clip : clipped)
                {
                    m_cam.ToRasterSpace(clip.verts[0].proj);
                    m_cam.ToRasterSpace(clip.verts[1].proj);
                    m_cam.ToRasterSpace(clip.verts[2].proj);
                    if (clip.Setup(binID, binProjectedClip.size()))
                    {
                        binProjectedClip.push_back(clip);
                    }
                }
            }
        }
    });
}

void
RendererAVX::TiledRasterizationStage()
{
    Concurrent::ForEach(m_CoreIds.begin(), m_CoreIds.end(), [&](int bin)
    {
        std::vector<Triangle>& binProjectedClip = m_ProjectedClip[bin];
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

                    if (index >= m_Tiles.size())
                        continue;

                    Tile& t = m_Tiles[index];

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

void
RendererAVX::RasterizationStage()
{
    Concurrent::ForEach(m_Tiles.begin(), m_Tiles.end(), [&](Tile& tile)
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
                            SIMDFloat visible = m_DepthBuffer.DepthTest(pixelX, pixelY, depth, inTriangle);

                            if (SIMD::Any(visible))
                            {
                                m_DepthBuffer.UpdateBuffer(pixelX, pixelY, visible, depth);
                                SIMDPixel pixel = SIMDPixel(SIMDVec2(posX, posY), depth, alpha, beta, gamma, tri.BinID,
                                                            tri.BinIndex);
                                // We deferred the shading to the fragment shading stage
                                m_PixelBuffer.SetBuffer(pixelX, pixelY, pixel, visible);
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

    m_PixelBuffer.AccumulatePixel(m_DepthBuffer);
}

void
RendererAVX::FragmentShaderStage()
{

    AssetServer& loader = AssetServer::GetInstance();

    BlingPhongSIMD shader;

    Concurrent::ForEach(m_PixelBuffer.begin(), m_PixelBuffer.end(), [&](SIMDPixel& pixel)
    {
        Triangle& triangle = m_ProjectedClip[pixel.binId][pixel.binIndex];
        pixel.Interpolate(triangle);
        Texture& texture = loader.GetTexture(triangle.verts[0].tex_id);
        shader.Shade(pixel, m_Lights, texture, m_cam);
        SIMDShader::SetColorBuffer(pixel, m_ColorBuffer);
    });
}

void
RendererAVX::UpdateFrameBuffer()
{
    App::RenderTexture(m_ColorBuffer.GetBuffer());
}

void
RendererAVX::ClearBuffer()
{
    m_DepthBuffer.Clear();
    m_ColorBuffer.Clear();
    m_PixelBuffer.Clear();
    for (int i = 0; i < m_Tiles.size(); i++)
    {
        m_Tiles[i].Clear();
    }
    
    for (int i = 0; i < m_CoreIds.size(); i++)
    {
        m_ProjectedClip[i].clear();
    }
    // threading makes this slower surprisingly
    //Concurrent::ForEach(m_Tiles.begin(), m_Tiles.end(), [&](Tile& tile)
    //{
    //    tile.Clear();
    //});
    //Concurrent::ForEach(m_CoreIds.begin(), m_CoreIds.end(), [&](int bin)
    //{
    //    m_ProjectedClip[bin].clear();
    //});
}
