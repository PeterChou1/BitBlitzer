#pragma once

#include "Vec2.h"

enum ShapeType
{
    CircleShape = 0,
    PolygonShape = 1,
    AABB = 2
};



class Shape
{
public:
    float radius{};
    float width{};
    float height{};
    std::vector<Vec2> polygonPoints;
    std::vector<Vec2> debugPoints;
    std::vector<Vec2> contactPoints;
    Vec2 max{};
    Vec2 min{};


    Shape() = default;

    ShapeType GetShapeType() const
    {
        return ShapeEnum;
    }


    static Shape CreateCircle(float radius)
    {
        auto collider = Shape();
        collider.radius = radius;
        collider.ShapeEnum = CircleShape;
        return collider;
    }

    static Shape CreateAABB(float width, float height)
    {
        auto collider = Shape();
        collider.ShapeEnum = AABB;
        collider.min = Vec2(-width/2, -height/2);
        collider.max = Vec2(width/2, height/2);
        return collider;
    }


    static Shape CreateRect(float width, float height)
    {
        auto collider = Shape();
        collider.polygonPoints.push_back(Vec2(-width/2.0f, -height/2.0f));
        collider.polygonPoints.push_back(Vec2(width/2.0f, -height/2.0f));
        collider.polygonPoints.push_back(Vec2(width/2.0f, height/2.0f));
        collider.polygonPoints.push_back(Vec2(-width / 2.0f, height / 2.0f));
        collider.ShapeEnum = PolygonShape;
        return collider;
    }

private:
    ShapeType ShapeEnum{};
};