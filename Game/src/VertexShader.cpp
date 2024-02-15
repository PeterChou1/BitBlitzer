#include "stdafx.h"
#include "VertexShader.h"

#include "Concurrent.h"
#include "ECSManager.h"

extern ECSManager ECS;

VertexShader::VertexShader()
{
    m_VertexBuffer = ECS.GetResource<VertexBuffer>();
    m_cam = ECS.GetResource<Camera>();
}

void VertexShader::Shade()
{
    auto& buffer = m_VertexBuffer->Buffer;
    Concurrent::ForEach(buffer.begin(), buffer.end(), [&](Vertex& v)
    {
        v.Projection = m_cam->Proj * Vec4(m_cam->WorldToCamera(v.Position));
    });
}
