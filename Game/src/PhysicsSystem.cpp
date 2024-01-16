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
constexpr float GravityScale = 3.0f;
Vec2 Gravity = Vec2(0.0f, 10.0f * GravityScale);



void PhysicsSystem::SyncTransform()
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

}

void PhysicsSystem::ForwardTransform()
{
    for (auto& e : ECS.Visit<RigidBody, Transform>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
        // don't bother updating masses with infinite mass
        Transform& transform = ECS.GetComponent<Transform>(e);
        rigidbody.ForwardTransform(transform, XY);
    }
}

void PhysicsSystem::Update(float deltaTime)
{

    m_Accumulate += deltaTime;

    if (m_Accumulate > MaxTime)
        m_Accumulate = MaxTime;

    // Physics is updated once every 100ms
    while (m_Accumulate > StepTime)
    {
        SyncTransform();
        for (int i = 0; i < STEP_ITERATION; i++)
        {
            Step();
        }
        ForwardTransform();
        m_Accumulate -= MaxTime;
    }

}

void PhysicsSystem::Step()
{

    float deltaTime = dt / STEP_ITERATION;

    // Broad Phase Collision

    std::vector<Manifold> collisions;


    // Generate Collision info
    for (auto& e1 : ECS.Visit<RigidBody>())
    {
        for (auto& e2 : ECS.Visit<RigidBody>())
        {
            if (e1 == e2 || e1 > e2) continue;
            RigidBody& r1 = ECS.GetComponent<RigidBody>(e1);
            RigidBody& r2 = ECS.GetComponent<RigidBody>(e2);
            // don't bother resolving collision between two infinite mass 
            if (r1.InvMass() == 0.0 && r2.InvMass() == 0.0f) continue;

            Manifold m = Manifold(e1, e2);
            if (m.Collided) 
                collisions.push_back(m);
        }
    }
    // Integrate Forces
    for (auto& e : ECS.Visit<RigidBody>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
    
        if (rigidbody.InvMass() == 0.0) continue;
    
        rigidbody.Force += Gravity;
        rigidbody.Velocity -= rigidbody.Force * rigidbody.InvMass() * deltaTime;
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
        rigidbody.IntegrateVelocityAngular(deltaTime);
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
        rigidbody.Force = Vec2(0.0f, 0.0f);
    }
}
