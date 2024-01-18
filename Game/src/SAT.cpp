#include "stdafx.h"
#include "SAT.h"


std::pair<float, float> project(const std::vector<Vec2>& polygon, const Vec2& axis)
{
    float min = axis.Dot(polygon[0]);
    float max = min;
    for (const Vec2& p : polygon)
    {
        float projection = axis.Dot(p);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
    return {min, max};
}

std::pair<float, float> projectCircle(Vec2 center, float radius, Vec2 axis)
{
    Vec2 p1 = center + axis * radius;
    Vec2 p2 = center - axis * radius;
    float p1proj = p1.Dot(axis);
    float p2proj = p2.Dot(axis);
    return {std::min(p1proj, p2proj), std::max(p1proj, p2proj)};
}

// Check if projections on an axis overlap
bool overlap(const std::pair<float, float>& a, const std::pair<float, float>& b)
{
    return a.first <= b.second && a.second >= b.first;
}


bool FindMTVCircle(
    Vec2& center, float radius,
    std::vector<Vec2> poly,
    std::vector<Vec2> normals,
    Vec2& collisionNormal,
    float& overlapAmount
)
{
    overlapAmount = std::numeric_limits<float>::max();

    // Seperating Axis Thereom
    for (const Vec2& axis : normals)
    {
        bool flip = true;

        auto proj1 = project(poly, axis);
        auto proj2 = projectCircle(center, radius, axis);

        if (!overlap(proj1, proj2))
            return false; // No overlap found, polygons do not intersect

        float o = std::min(proj1.second, proj2.second) - std::max(proj1.first, proj2.first);

        if (proj1.second > proj2.second)
        {
            flip = !flip;
        }

        if ((proj1.first <= proj2.first && proj2.second <= proj1.second) ||
            (proj2.first <= proj1.first && proj1.second <= proj2.second))
        {
            float mins = std::abs(proj1.first - proj2.first);
            float maxs = std::abs(proj1.second - proj2.second);
            if (mins < maxs)
            {
                o += mins;
                flip = true;
            }
            else
            {
                o += maxs;
                flip = false;
            }
        }

        if (o < overlapAmount)
        {
            overlapAmount = o;
            collisionNormal = axis;
            if (flip)
            {
                collisionNormal = collisionNormal * -1;
            }
        }
    }
}


bool SATPolygon(
    const std::vector<Vec2>& poly1,
    const std::vector<Vec2>& poly2,
    const std::vector<Vec2>& normals,
    float& overlapAmount,
    Vec2& collisionNormal
)
{
    for (const Vec2& axis : normals)
    {
        bool flip = true;
        auto proj1 = project(poly1, axis);
        auto proj2 = project(poly2, axis);

        if (!overlap(proj1, proj2))
            return false; // No overlap found, polygons do not intersect

        float o = std::min(proj1.second, proj2.second) - std::max(proj1.first, proj2.first);

        if (proj1.second > proj2.second)
        {
            flip = !flip;
        }

        if ((proj1.first <= proj2.first && proj2.second <= proj1.second) ||
            (proj2.first <= proj1.first && proj1.second <= proj2.second))
        {
            float mins = std::abs(proj1.first - proj2.first);
            float maxs = std::abs(proj1.second - proj2.second);
            if (mins < maxs)
            {
                o += mins;
                flip = true;
            }
            else
            {
                o += maxs;
                flip = false;
            }
        }

        if (o < overlapAmount)
        {
            overlapAmount = o;
            collisionNormal = axis;
            if (flip)
            {
                collisionNormal = collisionNormal * -1;
            }
        }
    }

    return true;
}

bool FindMTVPolygon(
    const std::vector<Vec2>& poly1, const std::vector<Vec2>& poly2,
    const std::vector<Vec2>& normals1, const std::vector<Vec2>& normals2,
    Vec2& collisionNormal,
    float& overlapAmount
)
{
    overlapAmount = std::numeric_limits<float>::max();

    // Test all edges of both polygons
    if (!SATPolygon(poly1, poly2, normals1, overlapAmount, collisionNormal))
        return false;

    if (!SATPolygon(poly1, poly2, normals2, overlapAmount, collisionNormal))
        return false;

    return true;
}
