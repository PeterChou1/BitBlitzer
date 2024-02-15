//---------------------------------------------------------------------------------
// SAT.h
//---------------------------------------------------------------------------------
//
// Implements the Separating Axis Theorem to resolve polygon/polygon or polygon circle
// collision
//
#pragma once

#include <vector>
#include "Vec2.h"

/**
 * \brief Find the minimum translation vector (MTV)
 *        between a circle and a polygon
 * \param center center of circle
 * \param radius radius of circle
 * \param poly points on the polygon 
 * \param normals edge normals of the polygon
 * \param collisionNormal a reference to collision normal to output
 * \param overlapAmount penetration amount to output to
 * \return 
 */
bool FindMTVCircle(
    Vec2& center, float radius,
    std::vector<Vec2> poly,
    std::vector<Vec2> normals,
    Vec2& collisionNormal,
    float& overlapAmount
);

/**
 * \brief Find the minimum translation vector (MTV)
 *        between a polygon and a polygon
 * \param poly1 points on the polygon1
 * \param poly2 points on the polygon2
 * \param normals1 edge normals of the polygon1
 * \param normals2 edge normals of the polygon2
 * \param collisionNormal a reference to collision normal to output
 * \param overlapAmount penetration amount to output to
 * \return
 */
bool FindMTVPolygon(
    const std::vector<Vec2>& poly1, 
    const std::vector<Vec2>& poly2,
    const std::vector<Vec2>& normals1, 
    const std::vector<Vec2>& normals2,
    Vec2& collisionNormal,
    float& overlapAmount
);
