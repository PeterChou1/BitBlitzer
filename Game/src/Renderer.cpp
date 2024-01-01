#include "stdafx.h"
#include <algorithm>
#include "Renderer.h"
#include "Utils.h"
#include "Coordinator.h"
#include "Clipper.h"
#include "../App/AppSettings.h"
#include "../app/app.h"


extern Coordinator gCoordinator;

constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


void ComputeBarycentricCoordinates(const Vertex& v1, const Vertex& v2, const Vertex& v3, float x, float y, float& alpha,
                                   float& beta, float& gamma)
{
    float det = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);
    alpha = ((v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y)) / det;
    beta = ((v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y)) / det;
    gamma = 1.0f - alpha - beta;
}

void FillBottom(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex, DepthBuffer& depth, ColorBuffer& color)
{
    float delta = 1;
    float invslope1 = (v2.pos.x - v1.pos.x) / (v2.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;


    float curx1 = v1.pos.x;
    float curx2 = v1.pos.x;

    if (invslope1 > invslope2)
    {
        std::swap(invslope1, invslope2);
    }

    for (float y = v1.pos.y; y <= v2.pos.y; y += delta)
    {
        for (float x = curx1; x < curx2; x += delta)
        {
            // ... interpolate u and v (texture coordinates) and draw point

            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, y, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            Vec3 normal = v1.normal * alpha + v2.normal * beta + v3.normal * gamma;

            normal.x /= z;
            normal.y /= z;
            normal.z /= z;

            u /= z;
            v /= z;

            if (z > depth.GetBuffer(x, y))
            {
                float r = normal.x * 255, g = normal.y * 255, b = normal.z * 255;
                // tex.Sample(u, v, r, g, b);
                // App::DrawPoint(x, y, r / 255.0f, g / 255.0f, b / 255.0f);
                // color.SetColor(static_cast<int>(x), static_cast<int>(y), r, g, b);
                color.SetColor(static_cast<int>(x), static_cast<int>(y), r, g, b);
                depth.SetBuffer(static_cast<int>(x), static_cast<int>(y), z);
            }
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void FillTop(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex, DepthBuffer& depth, ColorBuffer& color)
{
    float delta = 1;
    float invslope1 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v2.pos.x) / (v3.pos.y - v2.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;

    float curx1 = v3.pos.x;
    float curx2 = v3.pos.x;

    if (invslope1 < invslope2)
    {
        std::swap(invslope1, invslope2);
    }

    for (float y = v3.pos.y; y > v1.pos.y; y -= delta)
    {
        for (float x = curx1; x < curx2; x += delta)
        {
            // ... interpolate u and v (texture coordinates) and draw point
            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, y, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            Vec3 normal =  v1.normal * alpha + v2.normal * beta + v3.normal * gamma;

            normal.x /= z;
            normal.y /= z;
            normal.z /= z;


            u /= z;
            v /= z;

            if (z > depth.GetBuffer(x, y))
            {
                float r = normal.x * 255, g = normal.y * 255, b = normal.z * 255;
                // tex.Sample(u, v, r, g, b);
                // App::DrawPoint(x, y, r / 255.0f, g / 255.0f, b / 255.0f);
                //color.SetColor(static_cast<int>(x), static_cast<int>(y), r, g, b);
                color.SetColor(static_cast<int>(x), static_cast<int>(y), r, g, b);
                depth.SetBuffer(static_cast<int>(x), static_cast<int>(y), z);
            }
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}


void Renderer::RenderTriangle(Triangle& tri, SimpleTexture& tex)
{
    // Sort vertices by y-coordinate
    std::sort(std::begin(tri.verts), std::end(tri.verts),
              [](const Vertex& a, const Vertex& b)
              {
                  return a.pos.y < b.pos.y;
              }
    );

    Vertex& v1 = tri.verts[0];
    Vertex& v2 = tri.verts[1];
    Vertex& v3 = tri.verts[2];

    // trivial reject
    if ((v1.pos.y == v2.pos.y && v2.pos.y == v3.pos.y) || v3.pos.y - v1.pos.y <= 1.0)
    {
        return;
    }

    if (v2.pos.y == v3.pos.y)
    {
        FillBottom(v1, v2, v3, tex, m_depth, m_color);
    }
    else if (v1.pos.y == v2.pos.y)
    {
        FillTop(v1, v2, v3, tex, m_depth, m_color);
    }
    else
    {
        // Interpolate to find the fourth vertex
        float t = (v2.pos.y - v3.pos.y) / (v1.pos.y - v3.pos.y);
        Vec3 pos4 = Vec3(v3.pos.x + (v1.pos.x - v3.pos.x) * t, v2.pos.y, 0);
        Vec2 tex4 = v3.tex + (v1.tex - v3.tex) * t;
        float w = v3.invW + (v1.invW - v3.invW) * t;
        Vertex v4(pos4, tex4);
        v4.invW = w;

        FillBottom(v1, v2, v3, tex, m_depth, m_color);
        FillTop(v2, v4, v3, tex, m_depth, m_color);
    }
}

void Renderer::DebugDraw(const Triangle& tri)
{
    Vec4 v1 = tri.verts[0].proj;
    Vec4 v2 = tri.verts[1].proj;
    Vec4 v3 = tri.verts[2].proj;

    App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
    App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);
    App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
}

void Renderer::Render()
{
    m_depth.ClearBuffer();
    m_color.ClearBuffer();

    std::vector<Triangle> TrianglesToRaster;
    SimpleTexture tex;

    // Geometric pipeline
    for (Entity const& e : Visit<Mesh, Transform, SimpleTexture>(gCoordinator))
    {
        Mesh& mesh = gCoordinator.GetComponent<Mesh>(e);
        tex = gCoordinator.GetComponent<SimpleTexture>(e);

        for (Triangle& tri : mesh.tris)
        {
            // transform triangle by model matrix
            Triangle translation = tri;

            Vec3 normal = (translation.verts[0].normal + translation.verts[1].normal + translation.verts[2].normal) * (
                1.0f / 3.0f);
            // back face culling
            if (normal.Dot(translation.verts[0].pos - m_cam.pos) > 0)
            {
                // transform triangle into camera space
                Triangle view = translation;
                view.verts[0].pos = m_cam.WorldToCamera(view.verts[0].pos);
                view.verts[1].pos = m_cam.WorldToCamera(view.verts[1].pos);
                view.verts[2].pos = m_cam.WorldToCamera(view.verts[2].pos);
                view.verts[0].proj = m_cam.proj * Vec4(view.verts[0].pos);
                view.verts[1].proj = m_cam.proj * Vec4(view.verts[1].pos);
                view.verts[2].proj = m_cam.proj * Vec4(view.verts[2].pos);
                // clip against the near plane
                TrianglesToRaster.push_back(view);
            }
        }
    }

    for (Triangle& triangle : TrianglesToRaster)
    {
        std::vector<Triangle> clipped = Clip(triangle);

        for (Triangle& clip : clipped)
        {
            m_cam.ToRasterSpace(clip.verts[0].proj);
            m_cam.ToRasterSpace(clip.verts[1].proj);
            m_cam.ToRasterSpace(clip.verts[2].proj);

            clip.verts[0].pos.x = clip.verts[0].proj.x;
            clip.verts[0].pos.y = clip.verts[0].proj.y;
            clip.verts[1].pos.x = clip.verts[1].proj.x;
            clip.verts[1].pos.y = clip.verts[1].proj.y;
            clip.verts[2].pos.x = clip.verts[2].proj.x;
            clip.verts[2].pos.y = clip.verts[2].proj.y;

            RenderTriangle(clip, tex);
        }
    }


    App::RenderTexture(m_color.GetBuffer());
}
