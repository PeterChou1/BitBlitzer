#pragma once
#include <vector>
#include "Vec2.h"


bool FindMTVCircle(
    Vec2& center, float radius,
    std::vector<Vec2> poly,
    Vec2& collisionNormal,
    float& overlapAmount
);


bool FindMTVPolygon(
    const std::vector<Vec2>& poly1, const std::vector<Vec2>& poly2,
    Vec2& collisionNormal,
    float& overlapAmount
);
