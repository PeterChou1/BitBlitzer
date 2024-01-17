#include "stdafx.h"
#include "DebugPhysicsRenderer.h"

#include "Shape.h"
#include "Concurrent.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Vertex.h"
#include "../App/app.h"

extern ECSManager ECS;

void RenderAABB(std::vector<Vertex>& vertexBuffer, 
                  Vec2 min, Vec2 max, Vec3 color)
{
    Vec3 tR = Vec3(max.X, max.Y, 5.0f);
    Vec3 tL = Vec3(min.X, max.Y, 5.0f);
    Vec3 bR = Vec3(max.X, min.Y, 5.0f);
    Vec3 bL = Vec3(min.X, min.Y, 5.0f);
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
        point1.X += pos.X; point1.Y += pos.Y;
        // Calculate the x and y coordinates for the next point
        theta = (i + 1) * increment;
        Vec3 point2 = Vec3(radius * cosf(theta), radius * sinf(theta), 5.0f);
        point2.X += pos.X; point2.Y += pos.Y;

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
    Vec3& color
)
{
    int polySize = polygonPoints.size();

    for (int i = 0; i < polySize; i++)
    {
        Vec2 pt1 = polygonPoints[i];
        Vec2 pt2 = polygonPoints[i + 1 == polySize ? 0 : i + 1];

        Vec3 point1 = Vec3(pt1.X, pt1.Y, 5.0);
        Vec3 point2 = Vec3(pt2.X, pt2.Y, 5.0);

        Vertex vertex1 = Vertex(point1);
        Vertex vertex2 = Vertex(point2);

        vertex1.color = color;
        vertex2.color = color;

        vertexBuffer.push_back(vertex1);
        vertexBuffer.push_back(vertex2);
    }


    for (Vec2& pt : debugPoints)
    {
        Vec3 point = Vec3(pt.X, pt.Y, 5.0);
        Vertex v = Vertex(point);
        v.color = Vec3(1.0, 1.0, 1.0);
        vertexBuffer.push_back(v);
    }
}

DebugPhysicsRenderer::DebugPhysicsRenderer()
{
    m_Cam = ECS.GetResource<Camera>();
}

void DebugPhysicsRenderer::Update(float deltaTime)
{
    accumulate += deltaTime / 1000.0f;

    if (accumulate > 0.100)
    {
        if (App::IsKeyPressed('M'))
        {
            float x = 0.0f, y = 0.0f;
            App::GetMousePos(x, y);
            Vec3 planePoint = Vec3(0, 0, 5);
            Vec3 planeNormal = Vec3(0, 0, -1);
            Vec3 point = m_Cam->ScreenSpaceToWorldPoint(x, y, planePoint, planeNormal);
            Entity meshEntity = ECS.CreateEntity();
            auto modelTransform = Transform(point, Quat(Vec3(0, 0, 1), 0.0));
            ECS.AddComponent<Transform>(meshEntity, modelTransform);
            ECS.AddComponent<RigidBody>(meshEntity, RigidBody(1.0f));
            ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
            accumulate = 0.0f;
        }

        if (App::IsKeyPressed('N'))
        {
            float x = 0.0f, y = 0.0f;
            App::GetMousePos(x, y);
            Vec3 planePoint = Vec3(0, 0, 5);
            Vec3 planeNormal = Vec3(0, 0, -1);
            Vec3 point = m_Cam->ScreenSpaceToWorldPoint(x, y, planePoint, planeNormal);
            Entity meshEntity = ECS.CreateEntity();
            auto modelTransform = Transform(point, Quat(Vec3(0, 0, 1), 0.0));
            ECS.AddComponent<Transform>(meshEntity, modelTransform);
            ECS.AddComponent<RigidBody>(meshEntity, RigidBody(1.0f, 1.0f));
            ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
            accumulate = 0.0f;
        }

    } 

    
}

void DebugPhysicsRenderer::Render()
{
    std::vector<Vertex> debugVertexBuffer;

    std::vector<Vertex> debugPoints;

    for (auto& e : ECS.Visit<RigidBody>())
    {
        RigidBody& r = ECS.GetComponent<RigidBody>(e);
        Shape& shape = r.Shape;

        RenderAABB(debugVertexBuffer, r.RigidBodyAABB.Min, r.RigidBodyAABB.Max, Vec3(0.0, 0.0, 1.0));

        switch (shape.GetShapeType())
        {
        case CircleShape:
            RenderCircle(debugVertexBuffer, shape.Radius, r.Position, r.Color);
            break;
        case PolygonShape:
            RenderPolygon(debugVertexBuffer, shape.PolygonPoints, shape.DebugPoints, r.Color);
            break;
        }

        for (Vec2& pts : shape.ContactPoints)
        {
            Vec3 point = Vec3(pts.X, pts.Y, 5.0);
            debugPoints.push_back(point);
        }
    }

    Concurrent::ForEach(debugVertexBuffer.begin(), debugVertexBuffer.end(), [&](Vertex& v)
    {
        v.proj = m_Cam->Proj * Vec4(m_Cam->WorldToCamera(v.pos));
    });



    int lineSize = debugVertexBuffer.size() / 2.0f;

    for (int i = 0; i < lineSize; i++)
    {
        int idx = i * 2;

        Vertex& start = debugVertexBuffer[idx];
        Vertex& end = debugVertexBuffer[idx + 1];

        start.PerspectiveDivision();
        end.PerspectiveDivision();

        m_Cam->ToRasterSpaceDebug(start.proj);
        m_Cam->ToRasterSpaceDebug(end.proj);

        float startX = start.proj.X;
        float startY = start.proj.Y;

        float endX = end.proj.X;
        float endY = end.proj.Y;

        Vec3 color = start.color;

        App::DrawLine(startX, startY, endX, endY, color.X, color.Y, color.Z);

    }

    Concurrent::ForEach(debugPoints.begin(), debugPoints.end(), [&](Vertex& v)
    {
        v.proj = m_Cam->Proj * Vec4(m_Cam->WorldToCamera(v.pos));
    });

    for (Vertex& v : debugPoints) 
    {
        v.PerspectiveDivision();
        m_Cam->ToRasterSpaceDebug(v.proj);
        App::DrawDot(v.proj.X, v.proj.Y, 0.01, 1.0, 0.0, 0.0);
    }

    std::string rSize = "Amount of RigidBodies: " + std::to_string(ECS.Visit<RigidBody>().size());
    App::Print(100, 150, rSize.c_str());

}
