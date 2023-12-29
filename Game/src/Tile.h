#pragma once
#include "Vec2.h"
#include "Mesh.h"
#include "../App/app.h"
#include <thread>
#include <vector>

constexpr int TileSizeX = 32;
constexpr int TileSizeY = 32;

constexpr int TileCountX = APP_VIRTUAL_WIDTH / TileSizeX;
constexpr int TileCountY = APP_VIRTUAL_HEIGHT / TileSizeY;



class Tile {
 public:
     Tile(Vec2& min, Vec2& max) : minRaster(min), maxRaster(max) 
     {
         int numCores = std::thread::hardware_concurrency();
         binTriangles.resize(numCores);
         // 4 corners of the tile
     };
 
     void Add(std::uint32_t bin, Triangle tri) 
     {
         binTriangles[bin].push_back(tri);
     }
 
     void Clear() 
     {
         for (auto& binTriangle : binTriangles) 
         {
             binTriangle.clear();
         }
     }
 
 
     int Size() {
 
         int size = 0;
         for (auto& binTriangle : binTriangles)
         {
             size += binTriangle.size();
         }
         return size;
     }
     
     void DebugDraw(float r, float g, float b) 
     {
         App::DrawTriangle(minRaster.x, minRaster.y, maxRaster.x, maxRaster.y, minRaster.x, maxRaster.y, r, g, b);
         App::DrawTriangle(minRaster.x, minRaster.y, maxRaster.x, maxRaster.y, maxRaster.x, minRaster.y, r, g, b);
 
         App::DrawLine(minRaster.x, minRaster.y, minRaster.x, maxRaster.y);
         App::DrawLine(maxRaster.x, maxRaster.y, minRaster.x, maxRaster.y);
     }
 
     Vec2 GetMin() {
         return minRaster;
     }
 
     Vec2 GetMax() {
         return maxRaster;
     }
 
     Vec2 GetCorner(int index) 
     {
         return minRaster + CornerIndex[index];
     }
 
     std::vector<std::vector<Triangle>>& GetBinTriangle() 
     {
         return binTriangles;
     }
 
private:
    Vec2 minRaster;
    Vec2 maxRaster;
    std::vector<std::vector<Triangle>> binTriangles;
    static Vec2 CornerIndex[4];
};
