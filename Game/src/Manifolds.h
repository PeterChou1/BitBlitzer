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
    bool Collided{};
    float Penetration{};
    Vec2 Normal{};
    std::vector<Vec2> ContactPoints;

};


