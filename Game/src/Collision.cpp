#include "stdafx.h"
#include <cmath>

#include "Collision.h"
#include "SAT.h"
#include "Utils.h"


bool Circle2CircleCollision(const Vec2& aPos, const Vec2& bPos, const Shape& a, const Shape& b)
{
    float r = a.Radius + b.Radius;
    r *= r;
    return std::pow(aPos.X - bPos.X, 2.0f) + std::pow(aPos.Y - bPos.Y, 2.0f) <= r;
}


void Circle2Circle(Manifold& m, RigidBody& A, RigidBody& B)
{
    const Shape& AShape = A.Shape;
    const Shape& BShape = B.Shape;
    const Vec2& APos = A.Position;
    const Vec2& BPos = B.Position;

    assert(AShape.GetShapeType() == CircleShape && BShape.GetShapeType() == CircleShape);

    m.Collided = Circle2CircleCollision(APos, BPos, AShape, BShape);

    if (!m.Collided)
        return;

    Vec2 n = APos - BPos;
    float r = AShape.Radius + BShape.Radius;

    float distance = n.GetMagnitude();

    if (distance != 0.0f)
    {
        m.Penetration = r - distance;
        m.Normal = n / distance;
        m.ContactPoints.push_back(BPos + n);
    }
    else
    {
        m.Penetration = AShape.Radius;
        m.Normal = Vec2(1, 0);
        m.ContactPoints.push_back(APos);
    }
}

struct Edge
{
    Vec2 v1{};
    Vec2 v2{};
    Vec2 max{};
    Vec2 edgeV{};

    Edge() = default;

    Edge(Vec2 max, Vec2 v1, Vec2 v2) :
        max(max),
        v1(v1), v2(v2),
        edgeV(v2 - v1)
    {
    }

};

void FindClosestEdgeToNormal(
    const Vec2& normal, const std::vector<Vec2>& poly,
    Edge& edge
)
{
    float max_proj = -std::numeric_limits<float>::infinity();
    int index = 0;
    for (int i = 0; i < poly.size(); i++)
    {
        float proj = normal.Dot(poly[i]);
        if (proj > max_proj)
        {
            max_proj = proj;
            index = i;
        }
    }

    Vec2 v = poly[index];
    Vec2 v1 = poly[(index + 1) % poly.size()];
    Vec2 v0 = poly[index == 0 ? poly.size() - 1 : index - 1];

    Vec2 l = v - v1;
    Vec2 r = v - v0;
    l.Normalize();
    r.Normalize();

    if (r.Dot(normal) <= l.Dot(normal))
    {
        edge = Edge(v, v0, v);
    }
    else
    {
        edge = Edge(v, v, v1);
    }
}

std::vector<Vec2> ClipPoints(Vec2& v1, Vec2& v2, Vec2& n, float o)
{
    std::vector<Vec2> clippedPoints;
    float d1 = n.Dot(v1) - o;
    float d2 = n.Dot(v2) - o;

    if (d1 >= 0.0) clippedPoints.push_back(v1);
    if (d2 >= 0.0) clippedPoints.push_back(v2);

    if (d1 * d2 < 0.0)
    {
        Vec2 e = v2 - v1;
        float u = d1 / (d1 - d2);
        e = e * u + v1;
        clippedPoints.push_back(e);
    }

    return clippedPoints;
}

void FindContactPoints(Edge& ref, Edge& inc, std::vector<Vec2>& contactPoints)
{
    Vec2 refv = ref.edgeV;
    refv.Normalize();
    float o1 = refv.Dot(ref.v1);

    auto cp1 = ClipPoints(inc.v1, inc.v2, refv, o1);

    if (cp1.size() < 2) return;

    float o2 = refv.Dot(ref.v2);

    auto cp2 = ClipPoints(cp1[0], cp1[1], refv * -1.0f, -o2);

    if (cp2.size() < 2) return;

    Vec2 refNorm = refv.Cross(-1.0);

    float maxDepth = refNorm.Dot(ref.max);

    if (refNorm.Dot(cp2[0]) - maxDepth >= 0.0)
    {
        contactPoints.push_back(cp2[0]);
    }
    if (refNorm.Dot(cp2[1]) - maxDepth >= 0.0)
    {
        contactPoints.push_back(cp2[1]);
    }

}

void Polygon2Polygon(Manifold& m, RigidBody& A, RigidBody& B)
{
    Shape& AShape = A.Shape;
    Shape& BShape = B.Shape;

    assert(AShape.GetShapeType() == PolygonShape && BShape.GetShapeType() == PolygonShape);

    const auto& polyA = AShape.PolygonPoints;
    const auto& polyB = BShape.PolygonPoints;

    // find Minimum Translation Vector (MTV)
    m.Collided = FindMTVPolygon(
        polyA, polyB,
        AShape.EdgeNormals, BShape.EdgeNormals,
        m.Normal, m.Penetration
    );

    if (!m.Collided)
        return;

    Edge AEdge{};
    Edge BEdge{};

    FindClosestEdgeToNormal(m.Normal * -1.0, polyA, AEdge);
    FindClosestEdgeToNormal(m.Normal, polyB, BEdge);

    Edge ref, inc;
    if (std::abs(AEdge.edgeV.Dot(m.Normal)) <=
        std::abs(BEdge.edgeV.Dot(m.Normal)))
    {
        ref = AEdge;
        inc = BEdge;
    }
    else
    {
        ref = BEdge;
        inc = AEdge;
    }
    std::vector<Vec2> contactPoints;
    FindContactPoints(ref, inc, contactPoints);
    m.ContactPoints = contactPoints;
}


void Circle2Polygon(Manifold& m, RigidBody& A, RigidBody& B)
{
    Shape& AShape = A.Shape;
    Shape& BShape = B.Shape;
    assert(AShape.GetShapeType() == PolygonShape && BShape.GetShapeType() == CircleShape);
    std::vector<Vec2>& poly = AShape.PolygonPoints;

    // Use SAT to find circle normal and penetration
    m.Collided = FindMTVCircle(
        B.Position, BShape.Radius, poly, AShape.EdgeNormals,
        m.Normal, m.Penetration
    );

    if (!m.Collided)
        return;

    float minSquaredDistance = std::numeric_limits<float>::infinity();

    for (int i = 0; i < poly.size(); i++)
    {
        Vec2& point1 = poly[i];
        Vec2& point2 = poly[(i + 1) % poly.size()];
        Vec2 contactPoint = Utils::PointToLineSegment(B.Position, point1, point2);
        float squaredDistance = (contactPoint - B.Position).GetMagnitudeSquared();

        if (squaredDistance < minSquaredDistance)
        {
            minSquaredDistance = squaredDistance;
            m.ContactPoints = {contactPoint};
        }
    }

}

void Polygon2Circle(Manifold& m, RigidBody& A, RigidBody& B)
{
    Circle2Polygon(m, B, A);
    m.Normal *= -1.0f;
}
