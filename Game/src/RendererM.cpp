#include "stdafx.h"
#include "RendererM.h"
#include "Concurrency.h"
#include "GraphicsBuffer.h"
#include "Camera.h"
#include <thread>
#include <iostream>
#include <algorithm>
#include "../App/AppSettings.h"
#include "../App/app.h"


extern Coordinator gCoordinator;

inline void InterpolateVertexAttributes(const Vertex& insideVtx, const Vertex& outsideVtx, Vertex& newVtx, float t) {
    newVtx.tex = insideVtx.tex + (outsideVtx.tex - insideVtx.tex) * t;
    newVtx.invW = insideVtx.invW + (outsideVtx.invW - insideVtx.invW) * t;
}

std::vector<Triangle> ClipTriangle(const Vec3& planePoint, const Vec3& planeNormal, const Triangle& clip) {
    Vertex insideVertices[3];
    Vertex outsideVertices[3];
    int insidePts = 0;
    int outSidePts = 0;
    std::vector<Triangle> clippedTriangles;

    // Classify each vertex of the triangle as inside or outside relative to the clipping plane
    for (int i = 0; i < 3; ++i) {
        float distance = Dist(clip.verts[i].pos, planeNormal, planePoint);
        if (distance >= 0) {
            insideVertices[insidePts] = clip.verts[i];
            insidePts++;
        }
        else {
            outsideVertices[outSidePts] = clip.verts[i];
            outSidePts++;
        }
    }

    // Handle different cases based on the number of vertices inside the clipping plane
    switch (insidePts) {
    case 0:
        // All vertices are outside, so no part of the triangle is inside
        break;
    case 3:
        // All vertices are inside, so add the entire triangle
        clippedTriangles.push_back(clip);
        break;
    case 1:
    {
        // One vertex inside: form a new smaller triangle
        float t = 0;
        Triangle newTriangle = clip;

        newTriangle.verts[0] = insideVertices[0];
        newTriangle.verts[1].pos = IntersectPlane(planePoint, planeNormal, newTriangle.verts[0].pos, outsideVertices[0].pos, t);
        InterpolateVertexAttributes(insideVertices[0], outsideVertices[0], newTriangle.verts[1], t);

        newTriangle.verts[2].pos = IntersectPlane(planePoint, planeNormal, newTriangle.verts[0].pos, outsideVertices[1].pos, t);
        InterpolateVertexAttributes(insideVertices[0], outsideVertices[1], newTriangle.verts[2], t);

        clippedTriangles.emplace_back(newTriangle);
    }
    break;
    case 2:
    {
        // Two vertices inside: form two new triangles
        Triangle newTriangle1 = clip, newTriangle2 = clip;
        float t1 = 0, t2 = 0;

        newTriangle1.verts[0] = insideVertices[0];
        newTriangle1.verts[1] = insideVertices[1];
        newTriangle1.verts[2].pos = IntersectPlane(planePoint, planeNormal, insideVertices[0].pos, outsideVertices[0].pos, t1);
        InterpolateVertexAttributes(insideVertices[0], outsideVertices[0], newTriangle1.verts[2], t1);

        newTriangle2.verts[0] = insideVertices[1];
        newTriangle2.verts[1] = newTriangle1.verts[2];
        newTriangle2.verts[2].pos = IntersectPlane(planePoint, planeNormal, insideVertices[1].pos, outsideVertices[0].pos, t2);
        InterpolateVertexAttributes(insideVertices[1], outsideVertices[0], newTriangle2.verts[2], t2);

        clippedTriangles.emplace_back(newTriangle1);
        clippedTriangles.emplace_back(newTriangle2);
    }
    break;
    default:
        // This should never happen
        assert(false && "Unexpected number of inside vertices");
        break;
    }

    return clippedTriangles;
}

void RendererM::Render()
{
	VertexShaderStage();
	ClippingStage();
	TiledRasterizationStage();
	RasterizationStage();
	FragmentShaderStage();
	UpdateFrameBuffer();
}

void RendererM::VertexShaderStage()
{

	GraphicsBuffer& g = GetFirstComponent<GraphicsBuffer>(gCoordinator);
	Camera& cam = GetFirstComponent<Camera>(gCoordinator);
	std::vector<Vertex>& vertexBuffer = g.GetVertexBuffer();
	std::vector<Vertex>& projectedVertexBuffer = g.GetProjectedVertex();
	Concurrency::ForEach(vertexBuffer.begin(), vertexBuffer.end(), [&](Vertex& v) {
        Transform modelTransform = gCoordinator.GetComponent<Transform>(v.id);
        Vertex projected = v.transform(modelTransform).transform(cam.world_to_cam);
        projected.projectedPosition = cam.proj * projected.pos;
		projectedVertexBuffer[v.index] = projected;
	});
}

void RendererM::ClippingStage()
{
	GraphicsBuffer& g = GetFirstComponent<GraphicsBuffer>(gCoordinator);
	Camera& cam = GetFirstComponent<Camera>(gCoordinator);

	int numCores = std::thread::hardware_concurrency();
	std::vector<std::uint32_t> coreIds;
	coreIds.resize(numCores);
	std::iota(coreIds.begin(), coreIds.end(), 0);
	int trianglesCount = g.GetTriangleCount();
	int coreInterval = (trianglesCount + numCores - 1) / numCores;

	std::vector<Vertex>& vertexBuffer = g.GetVertexBuffer();
	std::vector<std::uint32_t>& indexBuffer = g.GetIndexBuffer();
    std::vector<std::vector<Triangle>>& projectedClip = g.GetProjectedClipped();
    std::vector<Vertex>& projectedVertexBuffer = g.GetProjectedVertex();

   
    // std::cout << "clipping start" << std::endl;
	Concurrency::ForEach(coreIds.begin(), coreIds.end(), [&](int bin) {
		int start = bin * coreInterval;
		int end = (bin + 1) * coreInterval;

        std::vector<Triangle>& binProjectedClip = projectedClip[bin];
        std::vector<Triangle> worldSpaceTriangle;

		for (int i = start; i < end; i++) {

			if (3 * i + 2 > indexBuffer.size()) break;
			Vertex v1 = projectedVertexBuffer[indexBuffer[3 * i]];
			Vertex v2 = projectedVertexBuffer[indexBuffer[3 * i + 1]];
			Vertex v3 = projectedVertexBuffer[indexBuffer[3 * i + 2]];
            // back face culling
            Triangle t = Triangle(v1, v2, v3);
            std::vector<Triangle> triangles = ClipTriangle(Vec3(0, 0, cam.nearplane), Vec3(0, 0, 1), t);
            worldSpaceTriangle.insert(worldSpaceTriangle.end(), triangles.begin(), triangles.end());
            
		}
        
        for (Triangle& triangle : worldSpaceTriangle) {
            Vec4 v1 = cam.proj * Vec4(triangle.verts[0].pos);
            Vec4 v2 = cam.proj * Vec4(triangle.verts[1].pos);
            Vec4 v3 = cam.proj * Vec4(triangle.verts[2].pos);

            // Perspective division
            triangle.verts[0].pos = v1.ToVec3();
            triangle.verts[1].pos = v2.ToVec3();
            triangle.verts[2].pos = v3.ToVec3();



            triangle.verts[0].tex /= v1.w;
            triangle.verts[1].tex /= v2.w;
            triangle.verts[2].tex /= v3.w;

            triangle.verts[0].invW = 1.0f / v1.w;
            triangle.verts[1].invW = 1.0f / v2.w;
            triangle.verts[2].invW = 1.0f / v3.w;

            // triangle culling

            triangle.verts[0].pos.x = static_cast<int>((triangle.verts[0].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH);
            triangle.verts[0].pos.y = static_cast<int>((triangle.verts[0].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT);
            
            
            triangle.verts[1].pos.x = static_cast<int>((triangle.verts[1].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH);
            triangle.verts[1].pos.y = static_cast<int>((triangle.verts[1].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT);
            
            triangle.verts[2].pos.x = static_cast<int>((triangle.verts[2].pos.x + 1) * 0.5 * APP_VIRTUAL_WIDTH);
            triangle.verts[2].pos.y = static_cast<int>((triangle.verts[2].pos.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT);

            
            // clip against 4 planes
            std::list<Triangle> clipped;
            clipped.push_back(triangle);
            int clipcount = 1;
            for (int sideID = 0; sideID < 4; sideID++) {
                 while (clipcount > 0)
                 {
                     Triangle test = clipped.front();
                     clipped.pop_front();
                     clipcount--;
                     std::vector<Triangle> boxClipped;
             
                     switch (sideID)
                     {
                     case 0: boxClipped = ClipTriangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), test); break;
                     case 1: boxClipped = ClipTriangle(Vec3(0.0f, APP_VIRTUAL_HEIGHT - 1, 0.0f), Vec3(0.0f, -1.0f, 0.0f), test); break;
                     case 2: boxClipped = ClipTriangle(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), test); break;
                     case 3: boxClipped = ClipTriangle(Vec3(APP_VIRTUAL_WIDTH - 1, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), test); break;
                     }
                     clipped.insert(clipped.end(), boxClipped.begin(), boxClipped.end());
                 }
                 clipcount = clipped.size();
             }
            for (Triangle& tri : clipped) {
                tri.Setup();
                binProjectedClip.push_back(tri);
            }
        } 
	});
}

void RendererM::TiledRasterizationStage()
{
    // GraphicsBuffer& g = GetFirstComponent<GraphicsBuffer>(gCoordinator);
    // std::vector<std::vector<Triangle>>& projectedClip = g.GetProjectedClipped();
    // std::vector<Tile>& tiles = g.GetTiles();
    // int numCores = std::thread::hardware_concurrency();
    // std::vector<std::uint32_t> coreIds;
    // coreIds.resize(numCores);
    // std::iota(coreIds.begin(), coreIds.end(), 0);
    // 
    // 
    // Concurrency::ForEach(coreIds.begin(), coreIds.end(), [&](int bin) {
    //    std::vector<Triangle>& binProjectedClip = projectedClip[bin];
    //    for (auto& tri : binProjectedClip) {
    //        // iterate through triangle bounding box
    // 
    //        int startX = tri.minX / TileSizeX;
    //        int endX = (tri.maxX + TileSizeX - 1) / TileSizeX;
    // 
    //        int startY = tri.minY / TileSizeY;
    //        int endY = (tri.maxY + TileSizeY - 1) / TileSizeY;
    // 
    // 
    //        for (int x = startX; x < endX; x++) {
    //            for (int y = startY; y < endY; y++) {
    //                int indexX = x / TileSizeX;
    //                int indexY = y / TileSizeY;
    // 
    //                int index = y * TileCountX + x;
    // 
    //                if (index >= tiles.size()) continue;
    // 
    //                Tile& t = tiles[index];
    // 
    //                Vec2 corner0 = t.GetCorner(tri.rejectIndex0);
    //                Vec2 corner1 = t.GetCorner(tri.rejectIndex1);
    //                Vec2 corner2 = t.GetCorner(tri.rejectIndex2);
    // 
    //                float e0 = tri.EdgeFunc0(corner0);
    //                float e1 = tri.EdgeFunc1(corner1);
    //                float e2 = tri.EdgeFunc2(corner2);
    // 
    //                // trivial reject
    //                if (e0 > 0 || e1 > 0 || e2 > 0) {
    //                    continue;
    //                }
    //                t.Add(bin, tri);
    //                //Vec2 cornerA0 = t.GetCorner(tri.acceptIndex0);
    //                //Vec2 cornerA1 = t.GetCorner(tri.acceptIndex1);
    //                //Vec2 cornerA2 = t.GetCorner(tri.acceptIndex2);
    //                //
    //                //float eA0 = tri.EdgeFunc0(cornerA0);
    //                //float eA1 = tri.EdgeFunc1(cornerA1);
    //                //float eA2 = tri.EdgeFunc2(cornerA2);
    //                //// trivial accept partial accept
    //                //if (eA0 <= 0 && eA1 <= 0 && eA2 <= 0)
    //                //{
    //                //    t.DebugDraw(0, 1, 0);
    //                //}
    //                //else {
    //                //    t.DebugDraw(0, 0, 1);
    //                //} 
    //            }
    //        }
    // 
    // 
    //    }
    // });
}

void RendererM::RasterizationStage()
{
    GraphicsBuffer& g = GetFirstComponent<GraphicsBuffer>(gCoordinator);
    std::vector<std::vector<Triangle>>& projectedClip = g.GetProjectedClipped();
    std::vector<Tile>& tiles = g.GetTiles();
    //
    //Concurrency::ForEach(tiles.begin(), tiles.end(), [&](Tile tile) {
    //
    //    std::vector<std::vector<Triangle>> binTriangles = tile.GetBinTriangle();
    //    for (auto& binTriangle : binTriangles) {
    //        for (auto& tri : binTriangle) {
    //    
    //            Vec2 minPt = tile.GetMin();
    //            Vec2 maxPt = tile.GetMax();
    //            
    //            // iterate over pixels
    //            
    //            for (int pixelY = minPt.y; pixelY < maxPt.y; pixelY++) {
    //                for (int pixelX = minPt.x; pixelX < maxPt.x; pixelX++) {
    //                    Vec2 pixel = Vec2(pixelX, pixelY);
    //                    float eA0 = tri.EdgeFunc0(pixel);
    //                    float eA1 = tri.EdgeFunc1(pixel);
    //                    float eA2 = tri.EdgeFunc2(pixel);
    //            
    //                    // trivial accept
    //                    if (eA0 <= 0 && eA1 <= 0 && eA2 <= 0)
    //                    {
    //                        App::DrawPoint(pixelX, pixelY);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //});

    for (Tile& t : tiles) {
        t.Clear();
    }

    for (auto& p : projectedClip) {
        for (auto& tri : p) {
            Vec3 v1 = tri.verts[0].pos;
            Vec3 v2 = tri.verts[1].pos;
            Vec3 v3 = tri.verts[2].pos;
            App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
            App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);
            App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
        }
        p.clear();
    }


}

void RendererM::FragmentShaderStage()
{

}

void RendererM::UpdateFrameBuffer()
{

}
