#pragma once

#include <vector>
#include "Entity.h"
#include "RigidBody.h"
#include "Vec2.h"

class Manifold
{
public:
    Manifold(RigidBody& A, RigidBody& B);

    void ResolveCollision();

    void ResolveCollisionAngular();

    void PositionCorrection();

    RigidBody& A;
    RigidBody& B;
    bool Collided{};
    float Penetration{};
    Vec2 Normal{};
    std::vector<Vec2> ContactPoints;

};


