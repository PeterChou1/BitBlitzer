#include "stdafx.h"
#include <thread>
#include <iostream>
#include <algorithm>
#include "RendererM.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "SIMD.h"
#include "../App/AppSettings.h"
#include "../App/app.h"

// uncomment this if processor does not support avx2
#define SIMD


extern Coordinator gCoordinator;

RendererM::RendererM(GraphicsBuffer& g, Camera& cam, ColorBuffer& color) :
                m_vertexBuffer(g.GetVertexBuffer()), m_indexBuffer(g.GetIndexBuffer()),
                m_projectedVertexBuffer(g.GetProjectedVertex()), m_projectedClip(g.GetProjectedClipped()),
                m_tiles(g.GetTiles()), m_cam(cam), m_color(color)
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
	Concurrency::ForEach(m_vertexBuffer.begin(), m_vertexBuffer.end(), [&](Vertex& v) {
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
	Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin) {
		int start = bin * m_coreInterval;
		int end = (bin + 1) * m_coreInterval;

        std::vector<Triangle>& binProjectedClip = m_projectedClip[bin];
        std::vector<Triangle> worldSpaceTriangle;

		for (int i = start; i < end; i++) 
        {

			if (3 * i + 2 > m_indexBuffer.size()) break;
			Vertex v1 = m_projectedVertexBuffer[m_indexBuffer[3 * i]];
			Vertex v2 = m_projectedVertexBuffer[m_indexBuffer[3 * i + 1]];
			Vertex v3 = m_projectedVertexBuffer[m_indexBuffer[3 * i + 2]];
            // back face culling
            Triangle t = Triangle(v1, v2, v3);

            if (v1.normal.Dot(v1.pos - m_cam.pos) > 0) 
            {
                std::vector<Triangle> clipped = Clip(t);
                // Output projected position to raster space
                for (Triangle& clip : clipped) 
                {
                    m_cam.ToRasterSpace(clip.verts[0].proj);
                    m_cam.ToRasterSpace(clip.verts[1].proj);
                    m_cam.ToRasterSpace(clip.verts[2].proj);
                    clip.Setup();
                }
                binProjectedClip.insert(binProjectedClip.end(), clipped.begin(), clipped.end());
            }
		}
        

	});
}

void RendererM::TiledRasterizationStage()
{
    
    Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin) {
       std::vector<Triangle>& binProjectedClip = m_projectedClip[bin];
       for (auto& tri : binProjectedClip) {
           // iterate through triangle bounding box
    
           int startX = tri.minX / TileSizeX;
           int endX = (tri.maxX + TileSizeX - 1) / TileSizeX;
    
           int startY = tri.minY / TileSizeY;
           int endY = (tri.maxY + TileSizeY - 1) / TileSizeY;
    
    
           for (int x = startX; x < endX; x++) {
               for (int y = startY; y < endY; y++) {
                   int indexX = x / TileSizeX;
                   int indexY = y / TileSizeY;
    
                   int index = y * TileCountX + x;
    
                   if (index >= m_tiles.size()) continue;
    
                   Tile& t = m_tiles[index];
    
                   Vec2 corner0 = t.GetCorner(tri.rejectIndex0);
                   Vec2 corner1 = t.GetCorner(tri.rejectIndex1);
                   Vec2 corner2 = t.GetCorner(tri.rejectIndex2);
                   
                   float e0 = tri.EdgeFunc0(corner0);
                   float e1 = tri.EdgeFunc1(corner1);
                   float e2 = tri.EdgeFunc2(corner2);
                   
                   // trivial reject
                   if (e0 > 0 || e1 > 0 || e2 > 0) {
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

    Concurrency::ForEach(m_tiles.begin(), m_tiles.end(), [&](Tile& tile) {
        std::vector<std::vector<Triangle>> binTriangles = tile.GetBinTriangle();

        for (auto& binTriangle : binTriangles) {
            for (auto& tri : binTriangle) {
        
                Vec2 minPt = tile.GetMin();
                Vec2 maxPt = tile.GetMax();
                // Might cause a data race due to overlap with pixels? investigate further
                minPt = Vec2(
                    (std::min)((std::max)(minPt.x, static_cast<float>(tri.minX)), static_cast<float>(tri.maxX)), 
                    (std::min)((std::max)(minPt.y, static_cast<float>(tri.minY)), static_cast<float>(tri.maxY))
                );
                
                maxPt = Vec2(
                    (std::max)((std::min)(maxPt.x, static_cast<float>(tri.maxX)), static_cast<float>(tri.minX)),
                    (std::max)((std::min)(maxPt.y, static_cast<float>(tri.maxY)), static_cast<float>(tri.minY))
                );


                // iterate over pixels to determine which pixel belong in the triangle



#ifdef SIMD    
                SIMDTriangle triSIMD(tri);
                for (int pixelY = minPt.y; pixelY < maxPt.y; pixelY += SIMDPixel::pixelHeight) {
                    for (int pixelX = minPt.x; pixelX < maxPt.x; pixelX += SIMDPixel::pixelWidth) {
                        SIMDPixel pixel = SIMDPixel(pixelX, pixelY);
                        if (triSIMD.IsInTriangle(pixel)) 
                        {
                            if (pixel.mask.GetBit(0)) {
                                m_color.SetColor(pixel.position.x[0], pixel.position.y[0], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(1)) {
                                m_color.SetColor(pixel.position.x[1], pixel.position.y[1], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(2)) {
                                m_color.SetColor(pixel.position.x[2], pixel.position.y[2], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(3)) {
                                m_color.SetColor(pixel.position.x[3], pixel.position.y[3], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(4)) {
                                m_color.SetColor(pixel.position.x[4], pixel.position.y[4], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(5)) {
                                m_color.SetColor(pixel.position.x[5], pixel.position.y[5], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(6)) {
                                m_color.SetColor(pixel.position.x[6], pixel.position.y[6], 255, 255, 255);
                            }
                        
                            if (pixel.mask.GetBit(7)) {
                                m_color.SetColor(pixel.position.x[7], pixel.position.y[7], 255, 255, 255);
                            }
                        }

                    }
                }
#else
                for (int pixelY = minPt.y; pixelY < maxPt.y; pixelY++) {
                    for (int pixelX = minPt.x; pixelX < maxPt.x; pixelX++) {

                        Vec2 pixelE = Vec2(pixelX, pixelY);
                        float eA0 = tri.EdgeFunc0(pixelE);
                        float eA1 = tri.EdgeFunc1(pixelE);
                        float eA2 = tri.EdgeFunc2(pixelE);

                        constexpr float epsilon = -std::numeric_limits<float>::epsilon();

                        // trivial accept
                        if (eA0 <= 0 && eA1 <= 0 && eA2 <= 0 &&
                            (eA0 <= epsilon || eA1 <= epsilon || eA2 <= epsilon))
                        {
                            m_color.SetColor(pixelX, pixelY, 255, 255, 255);
                        }
                    }
                }
#endif
            }
        }
    });


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

    //for (auto& p : m_projectedClip) {
    //    for (auto& tri : p) {
    //        Vec4 v1 = tri.verts[0].projectedPosition;
    //        Vec4 v2 = tri.verts[1].projectedPosition;
    //        Vec4 v3 = tri.verts[2].projectedPosition;
    //        App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
    //        App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);
    //        App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
    //    }
    //}

    m_color.ClearBuffer();

    Concurrency::ForEach(m_tiles.begin(), m_tiles.end(), [&](Tile& tile) {
        tile.Clear();
    });

    Concurrency::ForEach(m_coreIds.begin(), m_coreIds.end(), [&](int bin) {
        m_projectedClip[bin].clear();
    });

}
