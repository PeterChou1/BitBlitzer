//---------------------------------------------------------------------------------
// PhysicsSystem.h
//---------------------------------------------------------------------------------
//
// Implements the Physics Pipeline and main class for simulating RigidBody Physics
//
#pragma once

#include <vector>

#include "ColliderCallbackSystem.h"
#include "RigidBody.h"


class PhysicsSystem
{
public:

    PhysicsSystem();

    void SyncData();

    void ForwardTransform();

    void Update(float deltaTime);

    void Step();

private:

    float m_Accumulate{};
    std::shared_ptr<ColliderCallbackSystem> m_CallbackSystem;
    std::vector<std::pair<Entity, RigidBody&>> m_RigidBodies;
    std::vector<Manifold> m_Collisions;
    // How much times the physics iteration is run per time step
    // increase for more accuracy at the cost performance
    constexpr static float STEP_ITERATION = 6;
};
