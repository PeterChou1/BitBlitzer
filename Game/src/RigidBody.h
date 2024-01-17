//---------------------------------------------------------------------------------
// RigidBody.h
//---------------------------------------------------------------------------------
//
// Represent a physics body in the physics System a rigid body can be a circle
// or any concave polygon
//
#pragma once


#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Shape.h"
#include "Transform.h"
#include "AABB.h"


class RigidBody
{
public:

    RigidBody() = default;

    /**
     * \brief 
     * \param radius 
     */
    RigidBody(float radius);

    /**
     * \brief Construct A Rectangle RigidBody Based on width and height
     * \param width 
     * \param height 
     */
    RigidBody(float width, float height);

    /**
     * \brief Construct a RigidBody based on 
     * \param polygons 
     */
    RigidBody(std::vector<Vec2> polygons);

    /**
     * \brief Setting the body to infinite mass
     * \param isStatic 
     */
    void SetStatic(bool isStatic);

    void SyncTransform(Transform& transform, SlicePlane plane);

    void ForwardTransform(Transform& transform, SlicePlane plane) const;

    void RecomputeAABB();

    void ApplyImpulse(const Vec2& impulse);

    void ApplyImpulseAngular(const Vec2& impulse, const Vec2& contactVector);

    void IntegrateVelocity(float deltaTime);

    void IntegrateVelocityAngular(float deltaTime);

    float InvMass() const;

    float InvInertia() const;

    float Restitution() const;

    // use to sync rigid body with their transform
    bool Initialized{};
    // specify if rigid body is a player or controlled by a script
    bool Controlled{};
    unsigned int BitMaskLayers{};
    // used for debugging
    Vec3 Color{};
    // mutable states
    AABB RigidBodyAABB{};
    Shape Shape{};
    Vec2 Position{};
    Vec2 Velocity{};
    Vec2 Force{};
    float Angular{};
    float AngularVelocity{};
    float AngularDelta{};

private:
    // immutable states
    float m_InvInertia{};
    float m_InvMass{};
    float m_Restitution{};

    static constexpr float DEFAULT_DENSITY = 1.0f;
    static constexpr float DEFAULT_RESTITUTION = 0.5f;

};
