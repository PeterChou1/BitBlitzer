#pragma once
#include "Mesh.h"
#include <thread>
#include <vector>
#include "Tile.h"

class GraphicsBuffer
{
public:
    GraphicsBuffer() = default;

    GraphicsBuffer(const int width, const int height)
    {
        const unsigned int numCores = std::thread::hardware_concurrency();
        projectedClipped.resize(numCores);
        for (int y = 0; y < TileCountY; ++y)
        {
            for (int x = 0; x < TileCountX; ++x)
            {
                auto max = Vec2((std::min)((x + 1) * TileSizeX, width), (std::min)((y + 1) * TileSizeY, height));
                auto min = Vec2((std::min)(x * TileSizeX, width), (std::min)(y * TileSizeY, height));
                Tile t = Tile(min, max);
                tiles.push_back(t);
            }
        }
    }

    void AddMeshInstance(MeshInstance& mesh)
    {
        const int offsetVertex = vertices.size();
        for (auto& vertex : mesh.vertices)
        {
            vertex.index = offsetVertex + vertex.index;
            vertices.push_back(vertex);
        }
        for (const auto id : mesh.indices)
        {
            indices.push_back(offsetVertex + id);
        }
        projectedVertex.resize(vertices.size());
        triangleCount = indices.size() / 3;
    };

    std::uint32_t GetTriangleCount() const
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
