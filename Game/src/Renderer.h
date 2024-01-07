#pragma once
#include <unordered_map>
#include <vector>

#include "Camera.h"
#include "ColorBuffer.h"
#include "Entity.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Vertex.h"
#include "../App/app.h"

using BufferRange = std::pair<int, int>;


class Renderer
{
public:
    Renderer(int width, int height, Camera& cam) : m_cam(cam), m_ColorBuffer(ColorBuffer(width, height))
    {
        // Setup OpenGL texture to render to
        App::SetupGL();
    }

    void Update();

    virtual void Render() = 0;

protected:

    void DeleteMeshes(const std::vector<Entity>& entities);

    void UpdateMesh(Entity entity, Transform& transform);

    void AddMesh(Entity entity, Mesh mesh, Transform& transform);

    Camera& m_cam;
    std::vector<Vec3> m_Lights;
    std::vector<Vertex> m_ProjectedVertexBuffer;
    std::uint32_t m_TriangleCount{};
    std::vector<Vertex> m_VertexBuffer;
    std::vector<std::uint32_t> m_IndexBuffer;
    std::unordered_map<Entity, BufferRange> m_EntityToVertexRange;
    std::unordered_map<Entity, BufferRange> m_EntityToIndexRange;
    ColorBuffer m_ColorBuffer;


};