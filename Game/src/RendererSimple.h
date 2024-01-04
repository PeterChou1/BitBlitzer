#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "Camera.h"
#include "ColorBuffer.h"
#include "Triangle.h"

/*
* /brief CPU ECS
*/

class RendererSimple : public System
{
public:
    RendererSimple(Camera& cam, DepthBuffer& depth, ColorBuffer& color) : m_cam(cam), m_depth(depth), m_color(color)
    {
    }

    void RenderTriangle(Triangle& tri, Texture& tex);

    void DebugDraw(const Triangle& tri);

    void Render();

private:
    Camera& m_cam;
    DepthBuffer& m_depth;
    ColorBuffer& m_color;
};
