#include "stdafx.h"
#include "Renderer.h"

#include <iostream>

#include "AssetServer.h"
#include "Coordinator.h"
#include "Utils.h"


extern Coordinator ECS;

void Renderer::Update()
{
    std::vector<Entity> MarkedForDeletion;

    for (const auto e : ECS.Visit<Transform, Mesh>())
    {
        auto& mesh = ECS.GetComponent<Mesh>(e);
        auto& transform = ECS.GetComponent<Transform>(e);

        if (!mesh.loaded)
        {
            AddMesh(e, mesh, transform);
            mesh.loaded = true;
        }
        else if (mesh.markedForDeletion)
        {
            MarkedForDeletion.push_back(e);
        }

        if (transform.IsDirty)
        {
            UpdateMesh(e, transform);
        }
    }
    if (!MarkedForDeletion.empty()) 
        DeleteMeshes(MarkedForDeletion);
}

void Renderer::UpdateMesh(Entity entity, Transform& transform)
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

void UpdateEntityRanges(std::unordered_map<Entity, BufferRange>& entityToRangeMap, const std::vector<BufferRange>& rangesToRemove)
{
    for (auto& pair : entityToRangeMap)
    {
        int adjustment = 0;
        auto& range = pair.second;
        for (const auto& removedRange : rangesToRemove)
        {
            if (removedRange.first < range.first)
            {
                adjustment += removedRange.second - removedRange.first;
            }
        }
        range.first -= adjustment;
        range.second -= adjustment;
    }
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

        ECS.RemoveComponent<Mesh>(entity);
    }

    // Step 2: Sort ranges to facilitate efficient removal
    auto rangeComparer = [](const BufferRange& a, const BufferRange& b) { return a.first < b.first; };
    std::sort(vertexRangesToRemove.begin(), vertexRangesToRemove.end(), rangeComparer);
    std::sort(indexRangesToRemove.begin(), indexRangesToRemove.end(), rangeComparer);



    for (auto& pair : m_EntityToVertexRange)
    {
        int adjustment = 0;
        auto& range = pair.second;
        for (const auto& removedRange : vertexRangesToRemove)
        {
            if (removedRange.first < range.first)
            {
                adjustment += removedRange.second - removedRange.first;
            }
        }
        if (adjustment > 0)
        {
            for (int i = range.first; i < range.second; i++)
            {
                m_VertexBuffer[i].index -= adjustment;
            }
            auto& indexRange = m_EntityToIndexRange[pair.first];
            for (int i = indexRange.first; i < indexRange.second; i++)
            {
                m_IndexBuffer[i] -= adjustment;
            }
            range.first -= adjustment;
            range.second -= adjustment;
        }
    }

    for (auto& pair : m_EntityToIndexRange)
    {
        int adjustment = 0;
        auto& range = pair.second;
        for (const auto& removedRange : indexRangesToRemove)
        {
            if (removedRange.first < range.first)
            {
                adjustment += removedRange.second - removedRange.first;
            }
        }
        if (adjustment > 0)
        {
            range.first -= adjustment;
            range.second -= adjustment;
        }
    }

    Utils::EraseRanges(vertexRangesToRemove, m_VertexBuffer);
    Utils::EraseRanges(indexRangesToRemove, m_IndexBuffer);

    m_TriangleCount = m_IndexBuffer.size() / 3;
    m_ProjectedVertexBuffer.resize(m_VertexBuffer.size());
}



void Renderer::AddMesh(Entity entity, Mesh mesh, Transform& transform)
{
    MeshInstance instance = AssetServer::GetInstance().GetObj(mesh.MeshType);
    instance.transform(transform);
    int offsetVertex = m_VertexBuffer.size();
    m_EntityToVertexRange[entity]= {m_VertexBuffer.size(), m_VertexBuffer.size() + instance.vertices.size()};
    m_EntityToIndexRange[entity] = {m_IndexBuffer.size(), m_IndexBuffer.size() + instance.indices.size() };

    for (auto& vertex : instance.vertices)
    {
        vertex.index = offsetVertex + vertex.index;
        m_VertexBuffer.push_back(vertex);
    }
    for (const auto id : instance.indices)
    {
        m_IndexBuffer.push_back(offsetVertex + id);
    }
    m_TriangleCount = m_IndexBuffer.size() / 3;
    m_ProjectedVertexBuffer.resize(m_VertexBuffer.size());

    assert(m_EntityToVertexRange[entity].first < m_VertexBuffer.size());
    assert(m_EntityToVertexRange[entity].second <= m_VertexBuffer.size());
    assert(m_EntityToIndexRange[entity].first < m_IndexBuffer.size());
    assert(m_EntityToIndexRange[entity].second <= m_IndexBuffer.size());

}
