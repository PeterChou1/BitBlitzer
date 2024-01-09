#include "stdafx.h"

#include <iostream>
#include <numeric>

#include "Renderer.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "Rasterizer.h"
#include "Shader.h"
#include "SIMDShader.h"
#include "Triangle.h"
#include "Utils.h"

constexpr float epsilon = -std::numeric_limits<float>::epsilon();

extern ECSManager ECS;

Renderer::Renderer(int width, int height, Camera& cam) : m_cam(cam)
{
    CreateTile(width, height);
    m_ColorBuffer = ColorBuffer(width, height);
    m_DepthBuffer = SIMDDepthBuffer(width, height);
    m_PixelBuffer = SIMDPixelBuffer(width, height);
    m_CoreCount = std::thread::hardware_concurrency();
    m_CoreIds.resize(m_CoreCount);
    m_ProjectedClip.resize(m_CoreCount);
    m_Lights.push_back({ Vec3(0, 5, 5), Vec3(1, 1, 1), 40.0});
    std::iota(m_CoreIds.begin(), m_CoreIds.end(), 0);
    // Setup OpenGL texture to render to
    App::SetupGL();
}

void Renderer::CreateTile(int width, int height)
{
    for (int y = 0; y < TILE_COUNT_Y; ++y)
    {
        for (int x = 0; x < TILE_COUNT_X; ++x)
        {
            auto max = Vec2((std::min)((x + 1) * TILE_SIZE_X, width), (std::min)((y + 1) * TILE_SIZE_Y, height));
            auto min = Vec2((std::min)(x * TILE_SIZE_X, width), (std::min)(y * TILE_SIZE_Y, height));
            auto t = Tile(min, max);
            m_Tiles.push_back(t);
        }
    }
}


void Renderer::Update()
{
    std::vector<Entity> MarkedForDeletion;

    for (const auto e : ECS.Visit<Transform, Mesh>())
    {
        auto& mesh = ECS.GetComponent<Mesh>(e);
        auto& transform = ECS.GetComponent<Transform>(e);
        ShaderAsset shaderID = DefaultShader;

        if (ECS.HasComponent<Shader>(e))
        {
            shaderID = ECS.GetComponent<Shader>(e).shaderID;
        }

        if (!mesh.loaded)
        {
            AddMesh(e, mesh, transform, shaderID);
            mesh.loaded = true;
        }
        else if (mesh.markedForDeletion)
        {
            MarkedForDeletion.push_back(e);
        }
        else if (transform.IsDirty)
        {
            UpdateMeshTransform(e, transform);
        }

        if (m_EntityToShaderAssets.count(e) != 0 &&
            m_EntityToShaderAssets[e] != shaderID)
        {
            UpdateMeshShader(e, shaderID);
        }

    }

    if (!MarkedForDeletion.empty()) 
        DeleteMeshes(MarkedForDeletion);
}

void Renderer::Render()
{
    VertexTransform();
    ClipTriangle();
    TileAssignment();
    Rasterize();
    FragmentShade();
    UpdateFrameBuffer();
    ClearBuffer();
}


void Renderer::VertexTransform()
{

    Concurrent::ForEach(m_VertexBuffer.begin(), m_VertexBuffer.end(), [&](Vertex& v)
    {
        //Vertex projected = v;
        v.proj = m_cam.proj * Vec4(m_cam.WorldToCamera(v.pos));

        //m_ProjectedVertexBuffer[v.index] = projected;
    });
}

void Renderer::ClipTriangle()
{
    Concurrent::ForEach(m_CoreIds.begin(), m_CoreIds.end(), [&](int binID)
    {
        const int start = binID * m_CoreInterval;
        const auto end = (binID + 1) * m_CoreInterval;
        Rendering::Clip(
            m_cam,
            m_ProjectedClip, 
            m_VertexBuffer,
            m_IndexBuffer,
            binID,
            start,
            end
        );
    });
}

void Renderer::TileAssignment()
{
    Concurrent::ForEach(m_CoreIds.begin(), m_CoreIds.end(), [&](int binID)
    {
        Rendering::AssignTile(binID, m_ProjectedClip, m_Tiles);
    });
}

void Renderer::Rasterize()
{

    Concurrent::ForEach(m_Tiles.begin(), m_Tiles.end(), [&](Tile& tile)
    {
        Rendering::RasterizeTile(tile, m_PixelBuffer, m_DepthBuffer);
    });
   
    m_PixelBuffer.AccumulatePixel(m_DepthBuffer);
}

void Renderer::FragmentShade()
{
    AssetServer& loader = AssetServer::GetInstance();

    //BlinnPhongSIMD shader;

    Concurrent::ForEach(m_PixelBuffer.begin(), m_PixelBuffer.end(), [&](SIMDPixel& pixel)
    {
        Triangle& triangle = m_ProjectedClip[pixel.binId][pixel.binIndex];
        pixel.Interpolate(triangle);
        Material& texture = loader.GetMaterial(triangle.GetTextureID());
        SIMDShader* shader = loader.GetShader(triangle.GetShaderID());
        shader->Shade(pixel, m_Lights, texture, m_cam);
        SIMDShader::SetColorBuffer(pixel, m_ColorBuffer);
    });
}

void Renderer::UpdateFrameBuffer()
{
    App::RenderTexture(m_ColorBuffer.GetBuffer());
}

void Renderer::ClearBuffer()
{
    m_DepthBuffer.Clear();
    m_ColorBuffer.Clear();
    m_PixelBuffer.Clear();
    for (int i = 0; i < m_Tiles.size(); i++)
    {
        m_Tiles[i].Clear();
    }

    for (int i = 0; i < m_CoreIds.size(); i++)
    {
        m_ProjectedClip[i].clear();
    }

}

void Renderer::UpdateMeshTransform(Entity entity, Transform& transform)
{
    BufferRange range = m_EntityToVertexRange[entity];
    auto begin = m_VertexBuffer.begin() + range.first;
    auto end = m_VertexBuffer.begin() + range.second;
    std::for_each(begin, end, [&](Vertex& v)
    {
        v.pos = transform.TransformVec3(v.localPosition);
        v.normal = transform.TransformNormal(v.localNormal);
    });
    transform.IsDirty = false;
}

void Renderer::UpdateMeshShader(Entity entity, ShaderAsset shaderID)
{
    BufferRange range = m_EntityToVertexRange[entity];
    auto begin = m_VertexBuffer.begin() + range.first;
    auto end = m_VertexBuffer.begin() + range.second;
    std::for_each(begin, end, [&](Vertex& v)
    {
        v.shader_id = shaderID;
    });
}



void Renderer::DeleteMeshes(const std::vector<Entity>& entities)
{
    std::vector<BufferRange> vertexRangesToRemove;
    std::vector<BufferRange> indexRangesToRemove;
    


    for (const auto& entity : entities)
    {
        if (m_EntityToVertexRange.count(entity))
        {
            vertexRangesToRemove.push_back(m_EntityToVertexRange[entity]);
            m_EntityToVertexRange.erase(entity);
        }

        if (m_EntityToIndexRange.count(entity))
        {
            indexRangesToRemove.push_back(m_EntityToIndexRange[entity]);
            m_EntityToIndexRange.erase(entity);
        }

        if (m_EntityToShaderAssets.count(entity))
        {
            m_EntityToShaderAssets.erase(entity);
        }

        ECS.RemoveComponent<Mesh>(entity);
    }

    // Step 2: Sort ranges to facilitate efficient removal
    auto rangeComparer = [](const BufferRange& a, const BufferRange& b) { return a.first < b.first; };
    std::sort(vertexRangesToRemove.begin(), vertexRangesToRemove.end(), rangeComparer);
    std::sort(indexRangesToRemove.begin(), indexRangesToRemove.end(), rangeComparer);


    for (auto& pair : m_EntityToVertexRange)
    {
        int vertexAdjustment = 0;
        int indexAdjustment = 0;

        auto& entity = pair.first;
        auto& indexRange = m_EntityToIndexRange[entity];
        auto& vertexRange = m_EntityToVertexRange[entity];


        for (const auto& removedRange : vertexRangesToRemove)
        {
            if (removedRange.first < vertexRange.first)
            {
                vertexAdjustment += removedRange.second - removedRange.first;
            }
        }
        for (const auto& removedRange : indexRangesToRemove)
        {
            if (removedRange.first < indexRange.first)
            {
                indexAdjustment += removedRange.second - removedRange.first;
            }
        }

        if (vertexAdjustment > 0)
        {
            for (int i = indexRange.first; i < indexRange.second; i++)
            {
                m_IndexBuffer[i] -= vertexAdjustment;
            }
            vertexRange.first -= vertexAdjustment;
            vertexRange.second -= vertexAdjustment;
            indexRange.first -= indexAdjustment;
            indexRange.second -= indexAdjustment;
        }
    }

    Utils::EraseRanges(vertexRangesToRemove, m_VertexBuffer);
    Utils::EraseRanges(indexRangesToRemove, m_IndexBuffer);

    m_TriangleCount = m_IndexBuffer.size() / 3;
    //m_ProjectedVertexBuffer.resize(m_VertexBuffer.size());
    m_CoreInterval = (m_TriangleCount + m_CoreCount - 1) / m_CoreCount;
}



void Renderer::AddMesh(Entity entity, Mesh mesh, Transform& transform, ShaderAsset shaderID)
{
    MeshInstance instance = AssetServer::GetInstance().GetObj(mesh.MeshType);
    // Transform Mesh Instance to World Space
    instance.transform(transform);
    // Get Vertex Buffer Offset
    int offsetVertex = m_VertexBuffer.size();

    m_EntityToVertexRange[entity]= {m_VertexBuffer.size(), m_VertexBuffer.size() + instance.vertices.size()};
    m_EntityToIndexRange[entity] = {m_IndexBuffer.size(), m_IndexBuffer.size() + instance.indices.size() };
    m_EntityToShaderAssets[entity] = shaderID;

    for (auto& vertex : instance.vertices)
    {
        vertex.shader_id = shaderID;
        m_VertexBuffer.push_back(vertex);
    }
    for (const auto id : instance.indices)
    {
        m_IndexBuffer.push_back(offsetVertex + id);
    }

    m_TriangleCount = m_IndexBuffer.size() / 3;
    // m_ProjectedVertexBuffer.resize(m_VertexBuffer.size());
    m_CoreInterval = (m_TriangleCount + m_CoreCount - 1) / m_CoreCount;
}
