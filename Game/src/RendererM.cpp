#include "stdafx.h"
#include <thread>
#include <iostream>
#include <algorithm>
#include "RendererM.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "../App/AppSettings.h"
#include "../App/app.h"


extern Coordinator gCoordinator;

RendererM::RendererM(GraphicsBuffer& g, Camera& cam) : 
m_vertexBuffer(g.GetVertexBuffer()), m_indexBuffer(g.GetIndexBuffer()),
m_projectedVertexBuffer(g.GetProjectedVertex()), m_projectedClip(g.GetProjectedClipped()),
m_tiles(g.GetTiles()), m_cam(cam)
{
    m_coreCount = std::thread::hardware_concurrency();
    m_coreInterval = (g.GetTriangleCount() + m_coreCount - 1) / m_coreCount;
    coreIds.resize(m_coreCount);
    std::iota(coreIds.begin(), coreIds.end(), 0);
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
        Transform modelTransform = gCoordinator.GetComponent<Transform>(v.id);
        Vertex projected = v;
        projected.pos = modelTransform.TransformVec3(projected.pos);
        projected.normal = modelTransform.TransformNormal(projected.normal);
        projected.projectedPosition = m_cam.proj * Vec4(m_cam.WorldToCamera(projected.pos));
        m_projectedVertexBuffer[v.index] = projected;
	});
}

void RendererM::ClippingStage()
{ 
    // std::cout << "clipping start" << std::endl;
	Concurrency::ForEach(coreIds.begin(), coreIds.end(), [&](int bin) {
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

            //if (v1.normal.Dot(v1.pos - m_cam.pos) > 0) 
            //{
                std::vector<Triangle> clipped = Clip(t);
                // Output projected position to raster space
                for (Triangle& clip : clipped) 
                {
                    m_cam.ToRasterSpace(clip.verts[0].projectedPosition);
                    m_cam.ToRasterSpace(clip.verts[1].projectedPosition);
                    m_cam.ToRasterSpace(clip.verts[2].projectedPosition);
                    clip.Setup();
                }
                binProjectedClip.insert(binProjectedClip.end(), clipped.begin(), clipped.end());
            //}
		}
        

	});
}

void RendererM::TiledRasterizationStage()
{
    
    Concurrency::ForEach(coreIds.begin(), coreIds.end(), [&](int bin) {
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

    Concurrency::ForEach(m_tiles.begin(), m_tiles.end(), [&](Tile tile) {
        std::vector<std::vector<Triangle>> binTriangles = tile.GetBinTriangle();
        for (auto& binTriangle : binTriangles) {
            for (auto& tri : binTriangle) {
        
                Vec2 minPt = tile.GetMin();
                Vec2 maxPt = tile.GetMax();
                
                // iterate over pixels
                
                for (int pixelY = minPt.y; pixelY < maxPt.y; pixelY++) {
                    for (int pixelX = minPt.x; pixelX < maxPt.x; pixelX++) {
                        Vec2 pixel = Vec2(pixelX, pixelY);
                        float eA0 = tri.EdgeFunc0(pixel);
                        float eA1 = tri.EdgeFunc1(pixel);
                        float eA2 = tri.EdgeFunc2(pixel);
                
                        // trivial accept
                        if (eA0 <= 0 && eA1 <= 0 && eA2 <= 0)
                        {
                            App::DrawPoint(pixelX, pixelY);
                        }
                    }
                }
            }
        }
    });

 


}

void RendererM::FragmentShaderStage()
{

}

void RendererM::UpdateFrameBuffer()
{

}

void RendererM::ClearBuffer()
{

    for (auto& p : m_projectedClip) {
        for (auto& tri : p) {
            Vec4 v1 = tri.verts[0].projectedPosition;
            Vec4 v2 = tri.verts[1].projectedPosition;
            Vec4 v3 = tri.verts[2].projectedPosition;
            App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
            App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);
            App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
        }
    }


    for (Tile& tile : m_tiles) {
        tile.Clear();
    }
    Concurrency::ForEach(coreIds.begin(), coreIds.end(), [&](int bin) {
        m_projectedClip[bin].clear();
    });

}
