#include "stdafx.h"
#include <cmath>
#include "Collision.h"


bool AABB2AABBCollision(Vec2& aPos, Vec2& bPos, Shape& a, Shape& b)
{
    Vec2 aMax = aPos + a.max;
    Vec2 aMin = aPos + a.min;

    Vec2 bMax = bPos + b.max;
    Vec2 bMin = bPos + b.min;

    if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
    if (aMax.y < bMin.y || aMin.y > bMax.y) return false;

    // No separating axis found, therefor there is at least one overlapping axis
    return true;
}


bool Circle2CircleCollision(const Vec2& aPos, const Vec2& bPos, const Shape& a, const Shape& b)
{
    float r = a.radius + b.radius;
    r *= r;
    return std::pow(aPos.x - bPos.x, 2.0f) + std::pow(aPos.y - bPos.y, 2.0f) <= r;
}



void Circle2Circle(Manifold& m, RigidBody& A, RigidBody& B)
{
    const Shape& AShape = A.Shape;
    const Shape& BShape = B.Shape;
    const Vec2& APos = A.Position;
    const Vec2& BPos = B.Position;

    assert(AShape.GetShapeType() == CircleShape && BShape.GetShapeType() == CircleShape);

    m.collided = Circle2CircleCollision(APos, BPos, AShape, BShape);

    if (!m.collided)
        return;

    Vec2 n = APos - BPos;
    float r = AShape.radius + BShape.radius;

    float distance = n.GetMagnitude();

    if (distance != 0.0f)
    {
        m.penetration = r - distance;
        m.normal = n / distance;
        m.contactPoints.push_back(BPos + n);
    }
    else
    {
        m.penetration = AShape.radius;
        m.normal = Vec2(1, 0);
        m.contactPoints.push_back(APos);
    }
}

std::vector<Vec2> TranslatePoints
(
    const std::vector<Vec2>& polygonPoints,
    const float angle,
    const Vec2& position
)
{
    std::vector<Vec2> translatedPolygon;
    int polySize = polygonPoints.size();
    Mat2 matrix = Mat2(
        Vec2(std::cos(angle), -std::sin(angle)),
        Vec2(std::sin(angle), std::cos(angle))
    );

    for (int i = 0; i < polySize; i++)
    {
        Vec2 points = polygonPoints[i];
        Vec2 translated = matrix * points + position;
        translatedPolygon.push_back(translated);
    }

    return translatedPolygon;
}


void PointToLineSegment(Vec2 point, Vec2 a, Vec2 b, Vec2& linePoint)
{
    Vec2 a2b = b - a;
    Vec2 a2point = point - a;

    float proj = a2point.Dot(a2b);
    float lengthSquared = a2b.GetMagnitudeSquared();
    float d = proj / lengthSquared;

    if (d <= 0)
    {
        linePoint = a;
    }
    else if (d >= 1)
    {
        linePoint = b;
    }
    else
    {
        linePoint = a + a2b * d;
    }

}

void FindContactPoints(
    const std::vector<Vec2>& poly1, const std::vector<Vec2>& poly2,
    Vec2& contactPt1, Vec2& contactPt2, int& contactCount
)
{
    contactCount = 0;

    contactPt1 = Vec2(0.0, 0.0);
    contactPt2 = Vec2(0.0, 0.0);
    float minDistSq = std::numeric_limits<float>::infinity();

    for (int i = 0; i < poly1.size(); i++)
    {

        Vec2 point = poly1[i];

        for (int j = 0; j < poly2.size(); j++)
        {
            Vec2 A = poly2[j];
            Vec2 B = poly2[(j + 1) % poly2.size()];
            Vec2 linePoint;
            PointToLineSegment(point, A, B, linePoint);
            float distSq = (linePoint - point).GetMagnitudeSquared();

            if (std::abs(distSq - minDistSq) < 0.001 &&
                !(linePoint == contactPt1) &&
                !(linePoint == contactPt2))
            {
                contactPt2 = linePoint;
                contactCount = 2;
            }
            else if (distSq < minDistSq)
            {
                contactCount = 1;
                contactPt1 = linePoint;
                minDistSq = distSq;
            }
        }
    }

    for (int i = 0; i < poly2.size(); i++)
    {

        Vec2 point = poly2[i];

        for (int j = 0; j < poly1.size(); j++)
        {
            Vec2 A = poly1[j];
            Vec2 B = poly1[(j + 1) % poly1.size()];
            Vec2 linePoint;
            PointToLineSegment(point, A, B, linePoint);
            float distSq = (linePoint - point).GetMagnitudeSquared();

            if (std::abs(distSq - minDistSq) < 0.001)
            {
                contactPt2 = linePoint;
                contactCount = 2;
            }
            else if (distSq < minDistSq)
            {
                contactCount = 1;
                contactPt1 = linePoint;
                minDistSq = distSq;
            }
        }
    }
}

std::pair<float, float> project(const std::vector<Vec2>& polygon, const Vec2& axis) {
    float min = axis.Dot(polygon[0]);
    float max = min;
    for (const Vec2& p : polygon) {
        float projection = axis.Dot(p);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
    return { min, max };
}

// Check if projections on an axis overlap
bool overlap(const std::pair<float, float>& a, const std::pair<float, float>& b) {
    return a.first <= b.second && a.second >= b.first;
}

// Calculate MTV between two polygons
bool findMTV(
    const std::vector<Vec2>& poly1, const std::vector<Vec2>& poly2,
    Vec2& collisionNormal,
    float& overlapAmount
) {
    overlapAmount = std::numeric_limits<float>::max();

    // Test all edges of both polygons
    std::vector<Vec2> edges;
    for (size_t i = 0; i < poly1.size(); i++)
        edges.push_back(poly1[i] - poly1[(i + 1) % poly1.size()]);
    for (size_t i = 0; i < poly2.size(); i++)
        edges.push_back(poly2[i] - poly2[(i + 1) % poly2.size()]);

    // Seperating Axis Thereom

    for (const Vec2& edge : edges) {
        bool flip = true;

        Vec2 axis = edge.Cross(-1.0).Normalize();
        auto proj1 = project(poly1, axis);
        auto proj2 = project(poly2, axis);

        if (!overlap(proj1, proj2))
            return false; // No overlap found, polygons do not intersect

        float o = std::min(proj1.second, proj2.second) - std::max(proj1.first, proj2.first);

        if (proj1.second > proj2.second) {
            flip = !flip;
        }

        if ((proj1.first <= proj2.first && proj2.second <= proj1.second) ||
            (proj2.first <= proj1.first && proj1.second <= proj2.second))
        {
            float mins = std::abs(proj1.first - proj2.first);
            float maxs = std::abs(proj1.second - proj2.second);
            if (mins < maxs) {
                o += mins;
                flip = true;
            }
            else {
                o += maxs;
                flip = false;
            }
        }

        if (o < overlapAmount) {
            overlapAmount = o;
            collisionNormal = axis;
            if (flip) {
                collisionNormal = collisionNormal * -1;
            }
        }
    }

    return true;
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

    auto polyA = TranslatePoints(AShape.polygonPoints, A.Angular, A.Position);
    auto polyB = TranslatePoints(BShape.polygonPoints, B.Angular, B.Position);

    // find Minimum Translation Vector (MTV)
    m.collided = findMTV(polyA, polyB, m.normal, m.penetration);

    if (!m.collided)
        return;

    Edge AEdge{};
    Edge BEdge{};

    FindClosestEdgeToNormal(m.normal * -1.0, polyA, AEdge);
    FindClosestEdgeToNormal(m.normal, polyB, BEdge);

    Edge ref, inc;
    if (std::abs(AEdge.edgeV.Dot(m.normal)) <=
        std::abs(BEdge.edgeV.Dot(m.normal)))
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
    assert(contactPoints.size() != 0.0);
    m.contactPoints = contactPoints;
    AShape.contactPoints = contactPoints;
}

void Circle2Polygon(Manifold& m, RigidBody& A, RigidBody& B)
{
    Shape& AShape = A.Shape;
    Shape& BShape = B.Shape;
    assert(AShape.GetShapeType() == PolygonShape && BShape.GetShapeType() == CircleShape);
}

void Polygon2Circle(Manifold& m, RigidBody& A, RigidBody& B)
{
    Circle2Polygon(m, B, A);
}
