#pragma once
#include <vector>

#include "RigidBody.h"


class PhysicsSystem
{
public:

    void SyncData();

    void ForwardTransform();

    void Update(float deltaTime);

    void Step();

private:

    float m_Accumulate{};
    std::vector<std::pair<Entity, RigidBody&>> m_RigidBodies;
    // How much times the physics iteration is run per time step
    // increase for more accuracy at the cost performance
    constexpr static float STEP_ITERATION = 6;
};
