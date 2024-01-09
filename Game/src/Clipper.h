#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "Camera.h"
#include "ClippedTriangleBuffer.h"
#include "IndexBuffer.h"
#include "RenderConstants.h"
#include "VertexBuffer.h"


class Clipper
{
public:
    Clipper();

    void Clip();

private:
    std::shared_ptr<RenderConstants> m_RenderConstants;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<Camera> m_Cam;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<ClippedTriangleBuffer> m_ClippedTriangleBuffer;

};
