#pragma once

#include <vector>
#include "Entity.h"
#include "Vec2.h"

class Manifold
{
public:
    Manifold(Entity A, Entity B);

    void ResolveCollision();

    void ResolveCollisionAngular();

    void PositionCorrection();

    Entity A;
    Entity B;
    bool collided{};
    float penetration{};
    Vec2 normal{};
    std::vector<Vec2> contactPoints;

};


