//---------------------------------------------------------------------------------
// Manifold.h
//---------------------------------------------------------------------------------
//
// A manifold represents Collision information generated whenever two shapes
// in the physics system
//
#pragma once

#include <vector>

#include "RigidBody.h"
#include "Vec2.h"

class Manifold
{
public:
    Manifold(RigidBody& A, RigidBody& B);

    /**
     * \brief Resolve collision while ignoring rotation
     */
    void ResolveCollision();

    /**
     * \brief Resolve Collision accounting for rotation
     */
    void ResolveCollisionAngular();

    /**
     * \brief Separates A and B so that they no longer are intersecting
     *        This is an important part of the physics pipeline or else
     *        The bodies will slowly sink at rest
     */
    void PositionCorrection();

    RigidBody& A;
    RigidBody& B;
    // whether or not A and B collided
    bool Collided{};
    // how much A has penetrated into B if collided
    float Penetration{};
    // collision normal
    Vec2 Normal{};
    // contact points used for applying impulses to when resolving collision
    std::vector<Vec2> ContactPoints;

};


