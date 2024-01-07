#pragma once
#include <unordered_map>
#include <vector>

#include "Camera.h"
#include "Clipper.h"
#include "ColorBuffer.h"
#include "Entity.h"
#include "Lights.h"
#include "Mesh.h"
#include "SIMDPixelBuffer.h"
#include "Tile.h"
#include "Triangle.h"
#include "Vertex.h"

using BufferRange = std::pair<int, int>;

class Renderer
{
public:
    Renderer(int width, int height, Camera& cam);

    void Update();

    void Render();

protected:
    void CreateTile(int width, int height);

    void VertexTransform();

    void ClipTriangle();

    void TileAssignment();

    void Rasterize();

    void FragmentShade();

    void UpdateFrameBuffer();

    void ClearBuffer();

    void DeleteMeshes(const std::vector<Entity>& entities);

    void UpdateMesh(Entity entity, Transform& transform);

    void AddMesh(Entity entity, Mesh mesh, Transform& transform);

    Camera& m_cam;

    std::vector<std::uint32_t> m_CoreIds;
    std::vector<PointLight> m_Lights;
    std::vector<Vertex> m_VertexBuffer;
    std::vector<std::uint32_t> m_IndexBuffer;
    std::vector<Vertex> m_ProjectedVertexBuffer;
    std::vector<std::vector<Triangle>> m_ProjectedClip;
    std::vector<Tile> m_Tiles;
    SIMDDepthBuffer m_DepthBuffer;
    SIMDPixelBuffer m_PixelBuffer;
    ColorBuffer m_ColorBuffer;
    std::unordered_map<Entity, BufferRange> m_EntityToVertexRange;
    std::unordered_map<Entity, BufferRange> m_EntityToIndexRange;
    int m_CoreInterval{};
    unsigned int m_CoreCount;
    std::uint32_t m_TriangleCount{};
};
