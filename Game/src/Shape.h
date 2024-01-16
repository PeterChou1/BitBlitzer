#pragma once

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
        return ShapeEnum;
    }


    static Shape CreateCircle(float radius)
    {
        auto collider = Shape();
        collider.Radius = radius;
        collider.ShapeEnum = CircleShape;
        return collider;
    }

    static Shape CreateRect(float width, float height)
    {
        auto collider = Shape();
        collider.PolygonPoints.push_back(Vec2(-width/2.0f, -height/2.0f));
        collider.PolygonPoints.push_back(Vec2(width/2.0f, -height/2.0f));
        collider.PolygonPoints.push_back(Vec2(width/2.0f, height/2.0f));
        collider.PolygonPoints.push_back(Vec2(-width / 2.0f, height / 2.0f));
        collider.ShapeEnum = PolygonShape;
        return collider;
    }


    float Radius{};
    float Width{};
    float Height{};
    std::vector<Vec2> PolygonPoints;
    std::vector<Vec2> DebugPoints;
    std::vector<Vec2> ContactPoints;
    Vec2 Max{};
    Vec2 Min{};

private:
    ShapeType ShapeEnum{};
};