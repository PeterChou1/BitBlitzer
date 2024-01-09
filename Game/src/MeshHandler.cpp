#include "stdafx.h"
#include "MeshHandler.h"

#include "AssetServer.h"
#include "ECSManager.h"
#include "Shader.h"
#include "Transform.h"
#include "Utils.h"

extern ECSManager ECS;

MeshHandler::MeshHandler()
{
    m_RenderConstants = ECS.GetResource<RenderConstants>();
    m_VertexBuffer = ECS.GetResource<VertexBuffer>();
    m_IndexBuffer = ECS.GetResource<IndexBuffer>();
}

void MeshHandler::Update()
{
    std::vector<Entity> MarkedForDeletion;
    auto& EntityToShaderAsset = m_RenderConstants->EntityToShaderAsset;

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

        if (EntityToShaderAsset.count(e) != 0 &&
            EntityToShaderAsset[e] != shaderID)
        {
            UpdateMeshShader(e, shaderID);
        }

    }

    if (!MarkedForDeletion.empty())
        DeleteMeshes(MarkedForDeletion);
}

void MeshHandler::DeleteMeshes(const std::vector<Entity>& entities)
{
    std::vector<BufferRange> vertexRangesToRemove;
    std::vector<BufferRange> indexRangesToRemove;
    auto& EntityToVertexRange = m_RenderConstants->EntityToVertexRange;
    auto& EntityToIndexRange = m_RenderConstants->EntityToIndexRange;
    auto& EntityToShaderAsset = m_RenderConstants->EntityToShaderAsset;

    for (const auto& entity : entities)
    {
        if (EntityToVertexRange.count(entity))
        {
            vertexRangesToRemove.push_back(EntityToVertexRange[entity]);
            EntityToVertexRange.erase(entity);
        }

        if (EntityToIndexRange.count(entity))
        {
            indexRangesToRemove.push_back(EntityToIndexRange[entity]);
            EntityToIndexRange.erase(entity);
        }

        if (EntityToShaderAsset.count(entity))
        {
            EntityToShaderAsset.erase(entity);
        }

        ECS.RemoveComponent<Mesh>(entity);
    }

    // Step 2: Sort ranges to facilitate efficient removal
    auto rangeComparer = [](const BufferRange& a, const BufferRange& b) { return a.first < b.first; };
    std::sort(vertexRangesToRemove.begin(), vertexRangesToRemove.end(), rangeComparer);
    std::sort(indexRangesToRemove.begin(), indexRangesToRemove.end(), rangeComparer);


    for (auto& pair : EntityToVertexRange)
    {
        int vertexAdjustment = 0;
        int indexAdjustment = 0;

        auto& entity = pair.first;
        auto& indexRange = EntityToIndexRange[entity];
        auto& vertexRange = EntityToVertexRange[entity];


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
                m_IndexBuffer->buffer[i] -= vertexAdjustment;
            }
            vertexRange.first -= vertexAdjustment;
            vertexRange.second -= vertexAdjustment;
            indexRange.first -= indexAdjustment;
            indexRange.second -= indexAdjustment;
        }
    }

    Utils::EraseRanges(vertexRangesToRemove, m_VertexBuffer->buffer);
    Utils::EraseRanges(indexRangesToRemove, m_IndexBuffer->buffer);

    m_RenderConstants->TriangleCount = m_IndexBuffer->buffer.size() / 3;
    unsigned int CoreCount = m_RenderConstants->CoreCount;
    m_RenderConstants->CoreInterval = (m_RenderConstants->TriangleCount + CoreCount - 1) / CoreCount;
}

void MeshHandler::UpdateMeshTransform(Entity entity, Transform& transform)
{
    BufferRange range = m_RenderConstants->EntityToVertexRange[entity];
    auto begin = m_VertexBuffer->buffer.begin() + range.first;
    auto end = m_VertexBuffer->buffer.begin() + range.second;
    std::for_each(begin, end, [&](Vertex& v)
        {
            v.pos = transform.TransformVec3(v.localPosition);
            v.normal = transform.TransformNormal(v.localNormal);
        });
    transform.IsDirty = false;
}

void MeshHandler::UpdateMeshShader(Entity entity, ShaderAsset shaderID)
{
    BufferRange range = m_RenderConstants->EntityToVertexRange[entity];
    auto begin = m_VertexBuffer->buffer.begin() + range.first;
    auto end = m_VertexBuffer->buffer.begin() + range.second;
    std::for_each(begin, end, [&](Vertex& v)
        {
            v.shader_id = shaderID;
        });

}

void MeshHandler::AddMesh(Entity entity, Mesh mesh, Transform& transform, ShaderAsset shaderID)
{
    auto& EntityToVertexRange = m_RenderConstants->EntityToVertexRange;
    auto& EntityToIndexRange = m_RenderConstants->EntityToIndexRange;
    auto& EntityToShaderAsset = m_RenderConstants->EntityToShaderAsset;

    MeshInstance instance = AssetServer::GetInstance().GetObj(mesh.MeshType);
    // Transform Mesh Instance to World Space
    instance.transform(transform);
    // Get Vertex Buffer Offset
    int offsetVertex = m_VertexBuffer->buffer.size();

    EntityToVertexRange[entity] = { m_VertexBuffer->buffer.size(), m_VertexBuffer->buffer.size() + instance.vertices.size() };
    EntityToIndexRange[entity] = { m_IndexBuffer->buffer.size(), m_IndexBuffer->buffer.size() + instance.indices.size() };
    EntityToShaderAsset[entity] = shaderID;

    for (auto& vertex : instance.vertices)
    {
        vertex.shader_id = shaderID;
        m_VertexBuffer->buffer.push_back(vertex);
    }
    for (const auto id : instance.indices)
    {
        m_IndexBuffer->buffer.push_back(offsetVertex + id);
    }

    m_RenderConstants->TriangleCount = m_IndexBuffer->buffer.size() / 3;
    unsigned int CoreCount = m_RenderConstants->CoreCount;
    m_RenderConstants->CoreInterval = (m_RenderConstants->TriangleCount + CoreCount - 1) / CoreCount;
}
