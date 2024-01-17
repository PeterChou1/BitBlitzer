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
constexpr float GravityScale = 5.0f;
Vec2 Gravity = Vec2(0.0f, -10.0f * GravityScale);


void PhysicsSystem::SyncData()
{
    // Sync Transform -> RigidBody
    // Sync ECS -> m_RigidBodies
    m_RigidBodies.clear();
    for (auto& e : ECS.Visit<RigidBody, Transform>())
    {
        RigidBody& rigidbody = ECS.GetComponent<RigidBody>(e);
        rigidbody.Color = Vec3(1.0f, 0.0f, 0.0f);
        m_RigidBodies.emplace_back(e, rigidbody);

        if (rigidbody.Controlled || !rigidbody.Initialized)
        {
            Transform& transform = ECS.GetComponent<Transform>(e);
            rigidbody.SyncTransform(transform, XY);
            rigidbody.RecomputeAABB();
            rigidbody.Shape.RecomputePoints(rigidbody.Angular, rigidbody.Position);
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
        SyncData();
        for (int i = 0; i < STEP_ITERATION; i++)
        {
            Step();
            ForwardTransform();
        }
        m_Accumulate -= MaxTime;
    }
}

void PhysicsSystem::Step()
{
    float deltaTime = dt / STEP_ITERATION;

    // Broad Phase Collision

    std::vector<Manifold> collisions;
    for (auto& collisionPair1 : m_RigidBodies)
    {
        for (auto& collisionPair2 : m_RigidBodies)
        {
            Entity e1 = collisionPair1.first;
            Entity e2 = collisionPair2.first;
            if (e1 == e2 || e1 > e2) continue;

            RigidBody& r1 = collisionPair1.second;
            RigidBody& r2 = collisionPair2.second;

            // don't bother resolving collision between two infinite mass 
            if (r1.InvMass() == 0.0 && r2.InvMass() == 0.0f) continue;

            if (AABBTest(r1.RigidBodyAABB, r2.RigidBodyAABB))
            {
                Manifold m = Manifold(r1, r2);
                if (m.Collided)
                    collisions.push_back(m);
            }
        }
    }

    // Integrate Forces
    for (auto& collisionPair : m_RigidBodies)
    {
        RigidBody& rigidbody = collisionPair.second;

        if (rigidbody.InvMass() == 0.0) continue;

        rigidbody.Force += Gravity;
        rigidbody.Velocity += rigidbody.Force * rigidbody.InvMass() * deltaTime;

    }

    // Resolve Collision
    for (auto& manifold : collisions)
    {
        manifold.ResolveCollisionAngular();
    }

    // Integrate Velocity
    for (auto& collisionPair : m_RigidBodies)
    {
        RigidBody& rigidbody = collisionPair.second;
        rigidbody.IntegrateVelocityAngular(deltaTime);
    }

    // Correct Position
    for (auto& manifold : collisions)
    {
        manifold.PositionCorrection();
    }


    // Clear All Forces
    // Sync Rigid Body -> Transform
    for (auto& collisionPair : m_RigidBodies)
    {
        RigidBody& rigidbody = collisionPair.second;
        rigidbody.Force = Vec2(0.0f, 0.0f);
        rigidbody.Shape.RecomputePoints(rigidbody.Angular, rigidbody.Position);
        rigidbody.RecomputeAABB();
    }
}
