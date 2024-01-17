//---------------------------------------------------------------------------------
// Tile.h
//---------------------------------------------------------------------------------
//
// Tile is used to segment the display so to allow multi-threaded rendering
// every core is assigned a tile so the rendering pipeline can remain lock free
// and not worry about data races
//
#pragma once

#include <thread>
#include <vector>

#include "../App/app.h"
#include "Vec2.h"
#include "MeshInstance.h"
#include "Triangle.h"


static constexpr int TILE_SIZE_X = 32;
static constexpr int TILE_SIZE_Y = 32;

static constexpr int TILE_COUNT_X = APP_VIRTUAL_WIDTH / TILE_SIZE_X;
static constexpr int TILE_COUNT_Y = APP_VIRTUAL_HEIGHT / TILE_SIZE_Y;


class Tile
{
public:
    Tile(Vec2& min, Vec2& max) : m_MinRaster(min), m_MaxRaster(max)
    {
        const unsigned numCores = std::thread::hardware_concurrency();
        m_BinTriangles.resize(numCores);
    }

    void Add(std::uint32_t bin, Triangle& tri)
    {
        m_BinTriangles[bin].push_back(tri);
    }

    void Clear()
    {
        for (auto& binTriangle : m_BinTriangles)
        {
            binTriangle.clear();
        }
    }


    int Size() const
    {
        int size = 0;
        for (auto& binTriangle : m_BinTriangles)
        {
            size += binTriangle.size();
        }
        return size;
    }

    void DebugDraw(float r, float g, float b) const
    {
        App::DrawTriangle(m_MinRaster.X, m_MinRaster.Y, m_MaxRaster.X, m_MaxRaster.Y, m_MinRaster.X, m_MaxRaster.Y, r, g, b);
        App::DrawTriangle(m_MinRaster.X, m_MinRaster.Y, m_MaxRaster.X, m_MaxRaster.Y, m_MaxRaster.X, m_MinRaster.Y, r, g, b);
        App::DrawLine(m_MinRaster.X, m_MinRaster.Y, m_MinRaster.X, m_MaxRaster.Y);
        App::DrawLine(m_MaxRaster.X, m_MaxRaster.Y, m_MinRaster.X, m_MaxRaster.Y);
    }

    Vec2 GetMin() const
    {
        return m_MinRaster;
    }

    Vec2 GetMax() const
    {
        return m_MaxRaster;
    }

    Vec2 GetCorner(int index) const
    {
        return m_MinRaster + CornerIndex[index];
    }

    std::vector<std::vector<Triangle>>& GetBinTriangle()
    {
        return m_BinTriangles;
    }

private:
    Vec2 m_MinRaster;
    Vec2 m_MaxRaster;
    std::vector<std::vector<Triangle>> m_BinTriangles;
    static Vec2 CornerIndex[4];
};
