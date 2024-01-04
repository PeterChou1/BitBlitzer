#include "stdafx.h"
#include "Renderer.h"

#include "AssetServer.h"
#include "Utils.h"


extern Coordinator gCoordinator;

void Renderer::Update()
{
    std::vector<Entity> MarkedForDeletion;

    for (const auto e : Visit<Transform, Mesh>(gCoordinator))
    {
        auto& mesh = gCoordinator.GetComponent<Mesh>(e);
        auto& transform = gCoordinator.GetComponent<Transform>(e);

        if (!mesh.loaded)
        {
            AddMesh(mesh, transform);
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

        gCoordinator.RemoveComponent<Mesh>(entity);
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
        range.first -= adjustment;
        range.second -= adjustment;
        if (adjustment > 0)
        {
            for (int i = range.first; i < range.second; i++)
            {
                m_VertexBuffer[i].index = i;
            }
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
        range.first -= adjustment;
        range.second -= adjustment;
        if (adjustment > 0)
        {
            for (int i = range.first; i < range.second; i++)
            {
                m_IndexBuffer[i] -= adjustment;
            }
        }
    }

    m_TriangleCount = m_IndexBuffer.size() / 3;
    m_ProjectedVertexBuffer.resize(m_VertexBuffer.size());
}



void Renderer::AddMesh(Mesh mesh, Transform& transform)
{
    MeshInstance instance = AssetServer::GetInstance().GetObj(mesh.MeshType);
    instance.transform(transform);
    int offsetVertex = m_VertexBuffer.size();
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

}
