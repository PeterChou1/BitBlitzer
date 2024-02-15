//---------------------------------------------------------------------------------
// AABB.h
//---------------------------------------------------------------------------------
//
// Axis Align Bounding Box Used for cheap broad phase intersection test
// before the narrow phase
//

#pragma once
#include <cassert>
#include <vector>

#include "Utils.h"
#include "Vec2.h"

struct AABB
{
    Vec2 OriginalMax{};
    Vec2 OriginalMin{};

    Vec2 Max{};
    Vec2 Min{};

    AABB() = default;

    /**
     * \brief Computes AABB for Circle
     * \param radius of circle
     */
    AABB(float radius)
    {
        OriginalMax = Vec2(radius, radius);
        OriginalMin = Vec2(-radius, -radius);
        Min = OriginalMin;
        Max = OriginalMax;
    }

    /**
     * \brief Compute AABB for a polygon
     * \param points points in the polygon
     */
    AABB(const std::vector<Vec2>& points)
    {
        assert(!points.empty() && "Points Empty");
        ComputeMaxPoints(points, OriginalMax, OriginalMin);
        Min = OriginalMin;
        Max = OriginalMax;
    }

    /**
     * \brief Recomputes AABB at the end of physics pipeline
     */
    void RecomputeAABB(const Vec2& newPosition, float newAngle, ShapeType shapeType)
    {
        Vec2 topRight = OriginalMax;
        Vec2 topLeft = Vec2(OriginalMin.X, OriginalMax.Y);
        Vec2 bottomLeft = OriginalMin;
        Vec2 bottomRight = Vec2(OriginalMax.X, OriginalMin.Y);
        std::vector<Vec2> points = {topRight, topLeft, bottomLeft, bottomRight};
        if (shapeType == CircleShape)
        {
            points[0] += newPosition;
            points[1] += newPosition;
            points[2] += newPosition;
            points[3] += newPosition;
        }
        else
        {
            points = Utils::TranslatePoints(points, newAngle, newPosition);
        }
        ComputeMaxPoints(points, Max, Min);
    }


    static void ComputeMaxPoints(const std::vector<Vec2>& points, Vec2& maxPoint, Vec2& minPoint)
    {
        maxPoint = points[0];
        minPoint = points[0];
        for (const Vec2& point : points) 
        {
            if (point.X < minPoint.X) minPoint.X = point.X;
            if (point.Y < minPoint.Y) minPoint.Y = point.Y;
            if (point.X > maxPoint.X) maxPoint.X = point.X;
            if (point.Y > maxPoint.Y) maxPoint.Y = point.Y;
        }
    }
};

/**
 * \brief AABB intersection test returns true depending if A and B
 *        are colliding
 */
inline bool AABBTest(AABB& A, AABB& B)
{
    if (A.Max.X < B.Min.X || A.Min.X > B.Max.X) return false;
    if (A.Max.Y < B.Min.Y || A.Min.Y > B.Max.Y) return false;
    return true;
}

/**
 * \brief AABB intersection test with a point
 */
inline bool AABBPoint(AABB& A, float x, float y)
{
    if (A.Min.X > x || x > A.Max.X) return false;
    if (A.Min.Y > y || y > A.Max.Y) return false;
    return true;
}