#pragma once

#include <thread>
#include <vector>

#include "../App/app.h"
#include "Vec2.h"
#include "MeshInstance.h"
#include "Triangle.h"


constexpr int TileSizeX = 32;
constexpr int TileSizeY = 32;

constexpr int TileCountX = APP_VIRTUAL_WIDTH / TileSizeX;
constexpr int TileCountY = APP_VIRTUAL_HEIGHT / TileSizeY;


class Tile
{
public:
    Tile(Vec2& min, Vec2& max) : minRaster(min), maxRaster(max)
    {
        const unsigned numCores = std::thread::hardware_concurrency();
        binTriangles.resize(numCores);
        // 4 corners of the tile
    };

    void Add(std::uint32_t bin, Triangle& tri)
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


    int Size() const
    {
        int size = 0;
        for (auto& binTriangle : binTriangles)
        {
            size += binTriangle.size();
        }
        return size;
    }

    void DebugDraw(float r, float g, float b) const
    {
        App::DrawTriangle(minRaster.x, minRaster.y, maxRaster.x, maxRaster.y, minRaster.x, maxRaster.y, r, g, b);
        App::DrawTriangle(minRaster.x, minRaster.y, maxRaster.x, maxRaster.y, maxRaster.x, minRaster.y, r, g, b);

        App::DrawLine(minRaster.x, minRaster.y, minRaster.x, maxRaster.y);
        App::DrawLine(maxRaster.x, maxRaster.y, minRaster.x, maxRaster.y);
    }

    Vec2 GetMin() const
    {
        return minRaster;
    }

    Vec2 GetMax() const
    {
        return maxRaster;
    }

    int GetID() const
    {
        return id;
    }

    Vec2 GetCorner(int index) const
    {
        return minRaster + CornerIndex[index];
    }

    std::vector<std::vector<Triangle>>& GetBinTriangle()
    {
        return binTriangles;
    }

private:
    int id;
    Vec2 minRaster;
    Vec2 maxRaster;
    std::vector<std::vector<Triangle>> binTriangles;
    static Vec2 CornerIndex[4];
};
