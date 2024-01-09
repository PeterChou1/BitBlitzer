#pragma once
#include <memory>

#include "Camera.h"
#include "VertexBuffer.h"

class VertexShader
{
public:
    VertexShader();

    void Shade();

private:
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<Camera> m_cam;
};
