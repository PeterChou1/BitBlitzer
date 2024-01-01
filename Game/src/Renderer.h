#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "Camera.h"
#include "ColorBuffer.h"

/*
* /brief CPU ECS
*/

class Renderer : public System
{
public:
    Renderer(Camera& cam, DepthBuffer& depth, ColorBuffer& color) : m_cam(cam), m_depth(depth), m_color(color)
    {
    };

    void RenderTriangle(Triangle& tri, SimpleTexture& tex);

    void DebugDraw(const Triangle& tri);

    void Render();

private:
    Camera& m_cam;
    DepthBuffer& m_depth;
    ColorBuffer& m_color;
};
