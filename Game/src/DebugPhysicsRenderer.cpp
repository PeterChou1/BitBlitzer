#include "stdafx.h"
#include "DebugPhysicsRenderer.h"

#include "Shape.h"
#include "Concurrent.h"
#include "ECSManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Vertex.h"
#include "../App/app.h"

extern ECSManager ECS;

void RenderAABB(std::vector<Vertex>& vertexBuffer, 
                  Vec2 min, Vec2 max, Vec3 color)
{
    Vec3 tR = Vec3(max.x, max.y, 5.0f);
    Vec3 tL = Vec3(min.x, max.y, 5.0f);
    Vec3 bR = Vec3(max.x, min.y, 5.0f);
    Vec3 bL = Vec3(min.x, min.y, 5.0f);
    Vertex topRight = tR;
    Vertex topLeft = tL;
    Vertex bottomRight = bR;
    Vertex bottomLeft = bL;

    topRight.color = color;
    topLeft.color = color;
    bottomRight.color = color;
    bottomLeft.color = color;

    // -- push back top right line --
    vertexBuffer.push_back(topRight);
    vertexBuffer.push_back(topLeft);

    vertexBuffer.push_back(topRight);
    vertexBuffer.push_back(bottomRight);

    vertexBuffer.push_back(bottomRight);
    vertexBuffer.push_back(bottomLeft);

    vertexBuffer.push_back(topLeft);
    vertexBuffer.push_back(bottomLeft);


}

void RenderCircle(std::vector<Vertex>& vertexBuffer, int radius, Vec2 pos, Vec3 color)
{
    const int segments = 36; // Increase for a smoother circle
    const float increment = 2.0f * 3.141f / segments; // Increment angle

    for (int i = 0; i < segments; ++i)
    {
        // Calculate the x and y coordinates for the current point
        float theta = i * increment;
        Vec3 point1 = Vec3(radius * cosf(theta), radius * sinf(theta), 5.0f);
        point1.x += pos.x; point1.y += pos.y;
        // Calculate the x and y coordinates for the next point
        theta = (i + 1) * increment;
        Vec3 point2 = Vec3(radius * cosf(theta), radius * sinf(theta), 5.0f);
        point2.x += pos.x; point2.y += pos.y;

        Vertex p1 = point1;
        Vertex p2 = point2;

        p1.color = color;
        p2.color = color;

        // Transform and add the points to the vertex buffer
        vertexBuffer.push_back(p1);
        vertexBuffer.push_back(p2);
    }
}

void RenderPolygon(
    std::vector<Vertex>& vertexBuffer, 
    std::vector<Vec2>& polygonPoints,
    std::vector<Vec2>& debugPoints,
    float angle,
    Vec2& position,
    Vec3& color
)
{
    std::vector<Vec2> translatedPolygon;
    Mat2 matrix = Mat2(
        Vec2(std::cos(angle), -std::sin(angle)),
        Vec2(std::sin(angle), std::cos(angle))
    );
    int polySize = polygonPoints.size();

    for (int i = 0; i < polySize; i++)
    {
        Vec2 points = polygonPoints[i];
        Vec2 translated =  matrix * points + position;
        translatedPolygon.push_back(translated);
    }


    for (int i = 0; i < polySize; i++)
    {
        Vec2 pt1 = translatedPolygon[i];
        Vec2 pt2 = translatedPolygon[i + 1 == polySize ? 0 : i + 1];

        Vec3 point1 = Vec3(pt1.x, pt1.y, 5.0);
        Vec3 point2 = Vec3(pt2.x, pt2.y, 5.0);

        Vertex vertex1 = Vertex(point1);
        Vertex vertex2 = Vertex(point2);

        vertex1.color = color;
        vertex2.color = color;

        vertexBuffer.push_back(vertex1);
        vertexBuffer.push_back(vertex2);

    }


    for (Vec2& pt : debugPoints)
    {
        Vec3 point = Vec3(pt.x, pt.y, 5.0);
        Vertex v = Vertex(point);
        v.color = Vec3(1.0, 1.0, 1.0);
        vertexBuffer.push_back(v);
    }
}

DebugPhysicsRenderer::DebugPhysicsRenderer()
{
    m_cam = ECS.GetResource<Camera>();
}

void DebugPhysicsRenderer::Render()
{
    std::vector<Vertex> debugVertexBuffer;

    std::vector<Vertex> debugPoints;

    for (auto& e : ECS.Visit<RigidBody>())
    {
        RigidBody& r = ECS.GetComponent<RigidBody>(e);
        Shape& shape = r.Shape;

        switch (shape.GetShapeType())
        {
        case CircleShape:
            RenderCircle(debugVertexBuffer, shape.radius, r.Position, r.Color);
            break;
        case AABB:
            RenderAABB(debugVertexBuffer, shape.min + r.Position, shape.max + r.Position, r.Color);
            break;
        case PolygonShape:
            RenderPolygon(debugVertexBuffer, shape.polygonPoints, shape.debugPoints, r.Angular, r.Position, r.Color);
            break;
        }

        for (Vec2& pts : shape.contactPoints)
        {
            Vec3 point = Vec3(pts.x, pts.y, 5.0);
            debugPoints.push_back(point);
        }
    }

    Concurrent::ForEach(debugVertexBuffer.begin(), debugVertexBuffer.end(), [&](Vertex& v)
    {
        v.proj = m_cam->proj * Vec4(m_cam->WorldToCamera(v.pos));
    });



    int lineSize = debugVertexBuffer.size() / 2.0f;

    for (int i = 0; i < lineSize; i++)
    {
        int idx = i * 2;

        Vertex& start = debugVertexBuffer[idx];
        Vertex& end = debugVertexBuffer[idx + 1];

        start.PerspectiveDivision();
        end.PerspectiveDivision();

        m_cam->ToRasterSpaceDebug(start.proj);
        m_cam->ToRasterSpaceDebug(end.proj);

        float startX = start.proj.x;
        float startY = start.proj.y;

        float endX = end.proj.x;
        float endY = end.proj.y;

        Vec3 color = start.color;

        App::DrawLine(startX, startY, endX, endY, color.x, color.y, color.z);

    }

    Concurrent::ForEach(debugPoints.begin(), debugPoints.end(), [&](Vertex& v)
    {
        v.proj = m_cam->proj * Vec4(m_cam->WorldToCamera(v.pos));
    });

    for (Vertex& v : debugPoints) 
    {
        v.PerspectiveDivision();
        m_cam->ToRasterSpaceDebug(v.proj);
        App::DrawDot(v.proj.x, v.proj.y, 0.01, 1.0, 0.0, 0.0);
    }



}
