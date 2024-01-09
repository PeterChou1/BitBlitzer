#pragma once
#include <memory>

#include "IndexBuffer.h"
#include "Mesh.h"
#include "RenderConstants.h"
#include "VertexBuffer.h"

class MeshHandler
{
public:
    MeshHandler();

    void Update();

private:

    void DeleteMeshes(const std::vector<Entity>& entities);

    void UpdateMeshTransform(Entity entity, Transform& transform);

    void UpdateMeshShader(Entity entity, ShaderAsset shaderID);

    void AddMesh(Entity entity, Mesh mesh, Transform& transform, ShaderAsset shaderID);

    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<RenderConstants> m_RenderConstants;
};
