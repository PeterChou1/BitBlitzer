//---------------------------------------------------------------------------------
// MeshHandler.h
//---------------------------------------------------------------------------------
//
// Handles all meshes information in the rendering system
// responsible for adding deleting meshes/updating shaders for a mesh
//
#pragma once

#include <memory>

#include "Camera.h"
#include "CubeMap.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "RenderConstants.h"
#include "Transform.h"
#include "VertexBuffer.h"

class MeshHandler
{
public:
    MeshHandler();

    void Update();

private:

    void UpdateCubeMapTransform();

    void DeleteMeshes(const std::vector<Entity>& entities);

    void UpdateMeshTransform(Entity entity, Transform& transform);

    void UpdateMeshShader(Entity entity, ShaderAsset shaderID);

    void AddMesh(Entity entity, Mesh mesh, Transform& transform, ShaderAsset shaderID);

    void AddMeshRigidBody(Entity entity, Mesh mesh, Transform& transform, ShaderAsset shaderID);

    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<RenderConstants> m_RenderConstants;
    std::shared_ptr<CubeMap> m_CubeMap;
    std::shared_ptr<Camera> m_Cam;
};
