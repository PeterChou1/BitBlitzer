#include "stdafx.h"
#include "SAT.h"


std::pair<float, float> project(const std::vector<Vec2>& polygon, const Vec2& axis)
{
    float min = axis.Dot(polygon[0]);
    float max = min;
    for (const Vec2& p : polygon) {
        float projection = axis.Dot(p);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
    return { min, max };
}

std::pair<float, float> projectCircle(Vec2 center, float radius, Vec2 axis)
{
    Vec2 p1 = center + axis * radius;
    Vec2 p2 = center - axis * radius;
    float p1proj = p1.Dot(axis);
    float p2proj = p2.Dot(axis);
    return { std::min(p1proj, p2proj), std::max(p1proj, p2proj) };
}

// Check if projections on an axis overlap
bool overlap(const std::pair<float, float>& a, const std::pair<float, float>& b) {
    return a.first <= b.second && a.second >= b.first;
}


bool FindMTVCircle(
    Vec2& center, float radius,
    std::vector<Vec2> poly,
    Vec2& collisionNormal,
    float& overlapAmount
)
{
    overlapAmount = std::numeric_limits<float>::max();

    std::vector<Vec2> edges;
    for (size_t i = 0; i < poly.size(); i++)
        edges.push_back(poly[i] - poly[(i + 1) % poly.size()]);

    // Seperating Axis Thereom
    for (const Vec2& edge : edges) {
        bool flip = true;

        Vec2 axis = edge.Cross(-1.0).Normalize();
        auto proj1 = project(poly, axis);
        auto proj2 = projectCircle(center, radius, axis);

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
}

bool FindMTVPolygon(
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
