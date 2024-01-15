#include "stdafx.h"

#include <iostream>

#include "PhysicsSystem.h"
#include "ECSManager.h"
#include "Manifolds.h"
#include "RigidBody.h"
#include "Transform.h"


extern ECSManager ECS;

constexpr float StepTime = 16.0f;
constexpr float dt = StepTime / 1000.0f;
constexpr float MaxTime = 50.0f;
constexpr float GravityScale = 2.0f;
Vec2 Gravity = Vec2(0.0f, 10.0f * GravityScale);

void PhysicsSystem::Update(float deltaTime)
{
    //std::cout << "deltaTime: " << deltaTime << "\n";

    accumulate += deltaTime;

    if (accumulate > MaxTime)
        accumulate = MaxTime;

    // Physics is updated once every 100ms
    while (accumulate > StepTime)
    {
        Step();
        accumulate -= MaxTime;
    }

}

void PhysicsSystem::Step()
{

    // Sync Transform -> RigidBody
    for (auto& e : ECS.Visit<RigidBody, Transform>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);

        rigidbody.Color = Vec3(1.0f, 0.0f, 0.0f);

        if (rigidbody.Controlled || !rigidbody.Initialized)
        {
            Transform& transform = ECS.GetComponent<Transform>(e);
            rigidbody.SyncTransform(transform, XY);
            if (!rigidbody.Initialized) rigidbody.Initialized = true;
        }
    }

    std::vector<Manifold> collisions;

    // Generate Collision info
    for (auto& e1 : ECS.Visit<RigidBody>())
    {
        for (auto& e2 : ECS.Visit<RigidBody>())
        {
            if (e1 == e2 || e1 > e2) continue;

            Manifold m = Manifold(e1, e2);
            if (m.collided) 
                collisions.push_back(m);
        }
    }
    // Integrate Forces
    for (auto& e : ECS.Visit<RigidBody>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
    
        if (rigidbody.InvMass() == 0.0) continue;
    
        rigidbody.Force += Gravity;
        rigidbody.Velocity -= rigidbody.Force * rigidbody.InvMass() * dt;
    }

    // Resolve Collision
    for (auto& manifold : collisions)
    {
        manifold.ResolveCollisionAngular();
    }

    // Integrate Velocity
    for (auto& e : ECS.Visit<RigidBody>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
        rigidbody.IntegrateVelocityAngular(dt);
    }

    // Correct Position
    for (auto& manifold : collisions)
    {
        manifold.PositionCorrection();
    }

    // Clear All Forces
    // Sync Rigid Body -> Transform
    for (auto& e : ECS.Visit<RigidBody, Transform>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
        // don't bother updating masses with infinite mass
        if (rigidbody.InvMass() != 0.0f)
        {
            rigidbody.Force = Vec2(0.0f, 0.0f);
            Transform& transform = ECS.GetComponent<Transform>(e);
            rigidbody.ForwardTransform(transform, XY);
        }
    }
}
