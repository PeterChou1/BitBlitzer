#pragma once
#include <vector>
#include "Vec2.h"


bool FindMTVCircle(
    Vec2& center, float radius,
    std::vector<Vec2> poly,
    std::vector<Vec2> normals,
    Vec2& collisionNormal,
    float& overlapAmount
);


bool FindMTVPolygon(
    const std::vector<Vec2>& poly1, 
    const std::vector<Vec2>& poly2,
    const std::vector<Vec2>& normals1, 
    const std::vector<Vec2>& normals2,
    Vec2& collisionNormal,
    float& overlapAmount
);
