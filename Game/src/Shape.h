//---------------------------------------------------------------------------------
// Shape.h
//---------------------------------------------------------------------------------
//
// Class used to represent a shape within rigid body stores
// all shape information in the class depending on what shape it is
//
#pragma once

#include "Utils.h"
#include "Vec2.h"

enum ShapeType
{
    CircleShape = 0,
    PolygonShape = 1
};



class Shape
{
public:

    Shape() = default;

    ShapeType GetShapeType() const
    {
        return m_ShapeEnum;
    }

    void RecomputePoints(float angle, Vec2 position)
    {
        PolygonPoints = Utils::TranslatePoints(
                LocalSpacePoints,
                angle,
                position
        );
        ComputeEdgeNormals();
    }

    void ComputeEdgeNormals()
    {
        EdgeNormals.clear();
        size_t polySize = PolygonPoints.size();
        for (size_t i = 0; i < PolygonPoints.size(); i++) {
            Vec2 edge = PolygonPoints[i] - PolygonPoints[(i + 1) % polySize];
            EdgeNormals.push_back(edge.Cross(-1.0).Normalize());
        }
    }


    static Shape CreateCircle(float radius)
    {
        auto collider = Shape();
        collider.Radius = radius;
        collider.m_ShapeEnum = CircleShape;
        return collider;
    }

    static Shape CreateRect(float width, float height)
    {
        auto collider = Shape();
        collider.LocalSpacePoints.push_back(Vec2(-width/2.0f, -height/2.0f));
        collider.LocalSpacePoints.push_back(Vec2(width/2.0f, -height/2.0f));
        collider.LocalSpacePoints.push_back(Vec2(width/2.0f, height/2.0f));
        collider.LocalSpacePoints.push_back(Vec2(-width / 2.0f, height / 2.0f));
        collider.PolygonPoints = collider.LocalSpacePoints;
        collider.m_ShapeEnum = PolygonShape;
        return collider;
    }


    float Radius{};
    float Width{};
    float Height{};
    std::vector<Vec2> PolygonPoints;
    std::vector<Vec2> EdgeNormals;
    std::vector<Vec2> LocalSpacePoints;
    std::vector<Vec2> DebugPoints;
    std::vector<Vec2> ContactPoints;
    Vec2 Max{};
    Vec2 Min{};

private:
    ShapeType m_ShapeEnum{};
};