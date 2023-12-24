#pragma once
#include "Mesh.h"
#include <thread>
#include <vector>
#include "Tile.h"

class GraphicsBuffer {
public:
    GraphicsBuffer() {}

    GraphicsBuffer(int width, int height) {
        int numCores = std::thread::hardware_concurrency();
        projectedClipped.resize(numCores);
        int TileCountX = width / TileSizeX;
        int TileCountY = height / TileSizeY;
        for (int y = 0; y < TileCountY; ++y) {
            for (int x = 0; x < TileCountX; ++x) {
                auto max = Vec2((std::min)((x + 1) * TileSizeX, width), (std::min)((y + 1) * TileSizeY, height));
                auto min = Vec2((std::min)(x * TileSizeX, width), (std::min)(y * TileSizeY, height));
                Tile t = Tile(min, max);
                tiles.push_back(t);
            }
        }
    }

    void AddMeshInstance(MeshInstance& mesh) 
    {
        vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
        int offset = indices.size();
        for (auto id : mesh.indices) {
            indices.push_back(offset + id);
        };
        projectedVertex.resize(vertices.size());
        triangleCount = indices.size() / 3;
    };

    std::uint32_t GetTriangleCount() 
    {
        return triangleCount;
    }

    std::vector<Vertex>& GetVertexBuffer() 
    {
        return vertices;
    }

    std::vector<std::uint32_t>& GetIndexBuffer() 
    {
        return indices;
    }

    std::vector<Vertex>& GetProjectedVertex() 
    {
        return projectedVertex;
    }

    std::vector<std::vector<Triangle>>& GetProjectedClipped() 
    {
        return projectedClipped;
    }

    std::vector<Tile>& GetTiles() 
    {
        return tiles;
    }

private:
    std::uint32_t triangleCount{};
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<Vertex> projectedVertex;
    std::vector<std::vector<Triangle>> projectedClipped;
    std::vector<int> colorBuffer;
    std::vector<Tile> tiles;
};