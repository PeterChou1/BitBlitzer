#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "../App/AppSettings.h"
#include "../app/app.h"
#include "Quat.h"
#include "Renderer.h"
#include "Utils.h"
#include "Coordinator.h"
#include "Camera.h"



extern Coordinator gCoordinator;

constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


inline void InterpolateVertexAttributes(const Vertex& insideVtx, const Vertex& outsideVtx, Vertex& newVtx, float t) {
    newVtx.tex = insideVtx.tex + (outsideVtx.tex - insideVtx.tex) * t;
    newVtx.invW = insideVtx.invW + (outsideVtx.invW - insideVtx.invW) * t;
}

constexpr uint8_t LEFT_PLANE = 1 << 0, RIGHT_PLANE = 1 << 1, DOWN_PLANE = 1 << 2, UP_PLANE = 1 << 3, NEAR_PLANE = 1 << 4, FAR_PLANE = 1 << 5;

float Dot(uint8_t planeId, const Vec4& v)
{
    switch (planeId) {
    case LEFT_PLANE:
        return v.x + v.w;        /* v * (1 0 0 1) left */
    case RIGHT_PLANE:
        return -v.x + v.w;       /* v * (-1 0 0 1) right */
    case DOWN_PLANE:
        return v.y + v.w;        /* v * (0 1 0 1) down*/
    case UP_PLANE:
        return -v.y + v.w;       /* v * (0 -1 0 1) up*/
    case FAR_PLANE:
        return v.z + v.w;        /* v * (0 0 1 1) far */
    case NEAR_PLANE:
        return -v.z + v.w;       /* v * (0 0 -1 1) near */
    default:
        assert(false && "unreachable code");
    }
}

float PointToPlane(uint8_t planeId, const Vec4& a, const Vec4& b) 
{
    float alpha = Dot(planeId, a);
    float beta = Dot(planeId, b);
    return alpha / (alpha - beta);
}


void Lerp(const Point& a, const Point& b, float alpha, Point& out)
{
    float a1 = 1.0f - alpha;
    out.position = a.position * a1 + b.position * alpha;
    out.weights = a.weights * a1 + b.weights * alpha;
}


uint8_t OutCode(const Vec4& v) {

    uint8_t outcode = 0;

    if (Dot(LEFT_PLANE, v) < 0) outcode |= LEFT_PLANE;
    if (Dot(RIGHT_PLANE, v) < 0) outcode |= RIGHT_PLANE;
    if (Dot(DOWN_PLANE, v) < 0) outcode |= DOWN_PLANE;
    if (Dot(UP_PLANE, v) < 0) outcode |= UP_PLANE;
    if (Dot(FAR_PLANE, v) < 0) outcode |= FAR_PLANE;
    if (Dot(NEAR_PLANE, v) < 0) outcode |= NEAR_PLANE;

    return outcode;
}

std::vector<Point> ClipPlane(uint32_t planeid, std::vector<Point>& points) 
{

    std::vector<Point> outPoints;

    for (int i = 0, j = 1; i < points.size(); i++, j++) 
    {
        if (i == points.size() - 1) j = 0;

        const auto& Apoint = points[i];
        const auto& Bpoint = points[j];


        float t = PointToPlane(planeid, Apoint.position, Bpoint.position);
        Point newpoint{};
        Lerp(points[i], points[j], t, newpoint);

        // is inside

        const auto isInsideA = Dot(planeid, Apoint.position) > 0;
        const auto isInsideB = Dot(planeid, Bpoint.position) > 0;


        if (isInsideA) {
            if (isInsideB) {
                outPoints.push_back(Bpoint);
            }
            else {
                outPoints.push_back(newpoint);
            }
        } else if (isInsideB) {
            outPoints.push_back(newpoint);
            outPoints.push_back(Bpoint);
        }
    }

    return outPoints;
}


std::vector<Triangle> Renderer::Clip(Triangle& clip)
{
    
    uint8_t clipcode1 = OutCode(clip.verts[0].projectedPosition);
    uint8_t clipcode2 = OutCode(clip.verts[1].projectedPosition);
    uint8_t clipcode3 = OutCode(clip.verts[2].projectedPosition);

    std::vector<Triangle> clipped;


    if (!(clipcode1 | clipcode2 | clipcode3)) {
        // trivial accept
        clip.PerspectiveDivision();
        clipped.push_back(clip);
    } else if (!(clipcode1 & clipcode2 & clipcode3)) {

        Vertex v0 = clip.verts[0];
        Vertex v1 = clip.verts[1];
        Vertex v2 = clip.verts[2];


        std::vector<Point> points = {
            Point(clip.verts[0].projectedPosition, {1, 0, 0}),
            Point(clip.verts[1].projectedPosition, {0, 1, 0}),
            Point(clip.verts[2].projectedPosition, {0, 0, 1})
        };
        uint32_t mask = clipcode1 | clipcode2 | clipcode3;
        if (mask & LEFT_PLANE) {
            points = ClipPlane(LEFT_PLANE, points);
        }

        if (mask & RIGHT_PLANE) {
            points = ClipPlane(RIGHT_PLANE, points);
        }

        if (mask & DOWN_PLANE) {
            points = ClipPlane(DOWN_PLANE, points);
        }

        if (mask & UP_PLANE) {
            points = ClipPlane(UP_PLANE, points);
        }

        if (mask & NEAR_PLANE) {
            points = ClipPlane(NEAR_PLANE, points);
        }

        if (mask & FAR_PLANE) {
            points = ClipPlane(FAR_PLANE, points);
        }

        // triangulate the points
        for (int j = 2; j < points.size(); j++)
        {
            Point& p1 = points[0];
            Point& p2 = points[j - 1];
            Point& p3 = points[j];

            Vertex n1 = v0 * p1.weights.x + v1 * p1.weights.y + v2 * p1.weights.z;
            n1.projectedPosition = p1.position;
            Vertex n2 = v0 * p2.weights.x + v1 * p2.weights.y + v2 * p2.weights.z;
            n2.projectedPosition = p2.position;
            Vertex n3 = v0 * p3.weights.x + v1 * p3.weights.y + v2 * p3.weights.z;
            n3.projectedPosition = p3.position;

            Triangle newtri = Triangle(n1, n2, n3);
            newtri.PerspectiveDivision();
            clipped.push_back(newtri);
        }
    }
    // o/w trivial reject


    return clipped;
}

void ComputeBarycentricCoordinates(const Vertex& v1, const Vertex& v2, const Vertex& v3, float x, float y, float& alpha, float& beta, float& gamma) {
    float det = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);
    alpha = ((v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y)) / det;
    beta = ((v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y)) / det;
    gamma = 1.0f - alpha - beta;
}

void FillBottom(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex, DepthBuffer& depth) {

    float delta = 1;
    float invslope1 = (v2.pos.x - v1.pos.x) / (v2.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;


    float curx1 = v1.pos.x;
    float curx2 = v1.pos.x;

    if (invslope1 > invslope2) {
        std::swap(invslope1, invslope2);
    }

    for (float y = v1.pos.y; y <= v2.pos.y; y += delta) {
        for (float x = curx1; x < curx2; x += delta) {
            // ... interpolate u and v (texture coordinates) and draw point
            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, y, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            u /= z;
            v /= z;

            if (z > depth.GetBuffer(x, y)) {
                float r, g, b;
                tex.Sample(u, v, r, g, b);
                App::DrawPoint(x, y, r, g, b);
                depth.SetBuffer(static_cast<int>(x), static_cast<int>(y), z);
            }
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void FillTop(Vertex& v1, Vertex& v2, Vertex& v3, SimpleTexture& tex, DepthBuffer& depth) {

    float delta = 1;
    float invslope1 = (v3.pos.x - v1.pos.x) / (v3.pos.y - v1.pos.y);
    float invslope2 = (v3.pos.x - v2.pos.x) / (v3.pos.y - v2.pos.y);
    invslope1 *= delta;
    invslope2 *= delta;

    float curx1 = v3.pos.x;
    float curx2 = v3.pos.x;

    if (invslope1 < invslope2) {
        std::swap(invslope1, invslope2);
    }

    for (float y = v3.pos.y; y > v1.pos.y; y -= delta) {
        for (float x = curx1; x < curx2; x += delta) {
            // ... interpolate u and v (texture coordinates) and draw point
            float alpha, beta, gamma;
            ComputeBarycentricCoordinates(v1, v2, v3, x, y, alpha, beta, gamma);
            float u = alpha * v1.tex.x + beta * v2.tex.x + gamma * v3.tex.x;
            float v = alpha * v1.tex.y + beta * v2.tex.y + gamma * v3.tex.y;
            float z = alpha * v1.invW + beta * v2.invW + gamma * v3.invW;
            u /= z;
            v /= z;

            if (z > depth.GetBuffer(x, y)) {
                float r, g, b;
                tex.Sample(u, v, r, g, b);
                App::DrawPoint(x, y, r, g, b);
                depth.SetBuffer(static_cast<int>(x), static_cast<int>(y), z);
            }
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}




void Renderer::RenderTriangle(Triangle& tri, SimpleTexture& tex, DepthBuffer& depth) {
    // Sort vertices by y-coordinate
    std::sort(std::begin(tri.verts), std::end(tri.verts),
        [](const Vertex& a, const Vertex& b) {
            return a.pos.y < b.pos.y;
        }
    );

    Vertex& v1 = tri.verts[0];
    Vertex& v2 = tri.verts[1];
    Vertex& v3 = tri.verts[2];

    // trivial reject
    if ((v1.pos.y == v2.pos.y && v2.pos.y == v3.pos.y) || v3.pos.y - v1.pos.y <= 1.0) {
        return;
    }

    if (v2.pos.y == v3.pos.y) {
        FillBottom(v1, v2, v3, tex, depth);
    }
    else if (v1.pos.y == v2.pos.y) {
        FillTop(v1, v2, v3, tex, depth);
    }
    else {
        // Interpolate to find the fourth vertex
        float t = (v2.pos.y - v3.pos.y) / (v1.pos.y - v3.pos.y);
        Vec3 pos4 = Vec3(v3.pos.x + (v1.pos.x - v3.pos.x) * t, v2.pos.y, 0);
        Vec2 tex4 = v3.tex + (v1.tex - v3.tex) * t;
        float w = v3.invW + (v1.invW - v3.invW) * t;
        Vertex v4(pos4, tex4);
        v4.invW = w;

        FillBottom(v1, v2, v3, tex, depth);
        FillTop(v2, v4, v3, tex, depth);
    }
}

void Renderer::DebugDraw(const Triangle& tri)
{
    Vec4 v1 = tri.verts[0].projectedPosition;
    Vec4 v2 = tri.verts[1].projectedPosition;
    Vec4 v3 = tri.verts[2].projectedPosition;

    App::DrawLine(v1.x, v1.y, v2.x, v2.y, 1, 0, 0);
    App::DrawLine(v1.x, v1.y, v3.x, v3.y, 1, 0, 0);    
    App::DrawLine(v2.x, v2.y, v3.x, v3.y, 1, 0, 0);
}

void Renderer::Render()
{
    Camera& cam = GetFirstComponent<Camera>(gCoordinator);
    DepthBuffer& depth = GetFirstComponent<DepthBuffer>(gCoordinator);


    depth.ClearBuffer();

    Vec3 light_direction = Vec3(0, -1.0, 0.0f);
    std::vector<Triangle> TrianglesToRaster;
    //TODO Refactor texture system
    SimpleTexture tex;
   
    // Geometric pipeline
    for (Entity const& e : Visit<Mesh, Transform, SimpleTexture>(gCoordinator)) {

        // std::cout << "Entity ID found: " << e << std::endl;
        Mesh& mesh = gCoordinator.GetComponent<Mesh>(e);
        Transform& t = gCoordinator.GetComponent<Transform>(e);
        tex = gCoordinator.GetComponent<SimpleTexture>(e);

        for (Triangle& tri : mesh.tris) {
            // transform triangle by model matrix
            Triangle translation = tri.transform(t);
            Vec3 normal;

            if (!mesh.hasNormal) {
                Vec3 lineA = translation.verts[0].pos - translation.verts[1].pos;
                Vec3 lineB = translation.verts[0].pos - translation.verts[2].pos;
                normal = lineB.Cross(lineA);
                normal.Normalize();
                translation.verts[0].normal = normal;
                translation.verts[1].normal = normal;
                translation.verts[2].normal = normal;
            } else {
                normal = (translation.verts[0].normal + translation.verts[1].normal + translation.verts[2].normal) * (1 / 3);
            }
            // back face culling
            if (normal.Dot(translation.verts[0].pos - cam.pos) > 0)
            {
                // transform triangle into camera space
                Triangle view = translation.transform(cam.world_to_cam);
                view.verts[0].projectedPosition = cam.proj * Vec4(view.verts[0].pos);
                view.verts[1].projectedPosition = cam.proj * Vec4(view.verts[1].pos);
                view.verts[2].projectedPosition = cam.proj * Vec4(view.verts[2].pos);
                // clip against the near plane
                TrianglesToRaster.push_back(view);
            }
        }
    }

    for (Triangle& triangle : TrianglesToRaster) {

        std::vector<Triangle> clipped = Clip(triangle);        
        
        for (Triangle& clip : clipped) 
        {
            cam.ToRasterSpace(clip.verts[0].projectedPosition);
            cam.ToRasterSpace(clip.verts[1].projectedPosition);
            cam.ToRasterSpace(clip.verts[2].projectedPosition);
            //DebugDraw(clip);
            clip.verts[0].pos.x = clip.verts[0].projectedPosition.x;
            clip.verts[0].pos.y = clip.verts[0].projectedPosition.y;
            clip.verts[1].pos.x = clip.verts[1].projectedPosition.x;
            clip.verts[1].pos.y = clip.verts[1].projectedPosition.y;
            clip.verts[2].pos.x = clip.verts[2].projectedPosition.x;
            clip.verts[2].pos.y = clip.verts[2].projectedPosition.y;
            RenderTriangle(clip, tex, depth);
        }

    }
}



