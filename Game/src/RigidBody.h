#pragma once

#include <cassert>
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "Shape.h"
#include "Transform.h"
#include "AABB.h"


/**
 * \brief Since the transform class is 3d we have three ways
 *        to slice the 3d coordinates to sync up with our 2d
 *        physics system
 */
enum SlicePlane
{
    XY,
    XZ,
    YZ
};

class RigidBody
{
public:
    // states that change

    RigidBody() = default;

    // Circle Constructor
    RigidBody(float radius)
    {
        Shape = Shape::CreateCircle(radius);
        RigidBodyAABB = AABB(radius);
        float area = radius * radius * 3.141;
        float mass = DEFAULT_DENSITY * area;
        float inertia = 0.5f * mass * radius * radius;
        m_InvMass = 1.0f / mass;
        m_InvInertia = 1.0f / inertia;
    }

    // Square Constructor
    RigidBody(float width, float height)
    {
        Shape = Shape::CreateRect(width, height);
        RigidBodyAABB = AABB(Shape.PolygonPoints);
        float area = width * height;
        float mass = area * DEFAULT_DENSITY;
        float inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
        m_InvMass = 1.0f / mass;
        m_InvInertia = 1.0f / inertia;
        m_Restitution = DEFAULT_RESTITUTION;
    }

    RigidBody(std::vector<Vec2> polygons)
    {
        assert(false && "not implemented");
    }

    void SetStatic(bool isStatic)
    {
        if (isStatic)
        {
            m_InvMass = 0.0f;
            m_InvInertia = 0.0f;
        }
    }


    void SyncTransform(Transform& transform, SlicePlane plane)
    {
        float roll, pitch, yaw;
        transform.Rotation.GetEulerAngles(roll, pitch, yaw);
        switch (plane)
        {
        case YZ:
            Position.X = transform.Position.Y;
            Position.Y = transform.Position.Z;
            Angular = roll;
            break;
        case XZ:
            Position.X = transform.Position.X;
            Position.Y = transform.Position.Z;
            Angular = pitch;
            break;
        case XY:
            Position.X = transform.Position.X;
            Position.Y = transform.Position.Y;
            Angular = yaw;
            break;
        }
    }

    void ForwardTransform(Transform& transform, SlicePlane plane) const
    {
        transform.SetPosition2D(Position);
        switch (plane)
        {
        case YZ:
            transform.UpdateRow(AngularDelta);
            break;
        case XZ:
            transform.UpdatePitch(AngularDelta);
            break;
        case XY:
            transform.UpdateYaw(AngularDelta);
        }
    }

    void RecomputeAABB()
    {
        RigidBodyAABB.RecomputeAABB(Position, Angular, Shape.GetShapeType());
    }

    void ApplyForce(const Vec2& f)
    {
        Force += f;
    }

    void IntegrateForce(float deltaTime)
    {
        if (m_InvMass == 0.0f) return;
        Velocity += Force * m_InvMass * deltaTime;
    }


    void ApplyImpulse(const Vec2& impulse)
    {
        Velocity += impulse * m_InvMass;
    }

    void ApplyImpulseAngular(const Vec2& impulse, const Vec2& contactVector)
    {
        Velocity += impulse * m_InvMass;
        AngularVelocity += contactVector.Cross(impulse) * m_InvInertia;
    }


    void IntegrateVelocity(float deltaTime)
    {
        if (m_InvMass == 0.0f) return;
        Position += Velocity * deltaTime;
    }


    void IntegrateVelocityAngular(float deltaTime)
    {
        if (m_InvMass == 0.0f) return;
        Position += Velocity * deltaTime;
        AngularDelta = AngularVelocity * deltaTime;
        Angular += AngularDelta;
    }

    float InvMass() const 
    {
        return m_InvMass;
    }

    float InvInertia() const
    {
        return m_InvInertia;
    }

    float Restitution() const
    {
        return m_Restitution;
    }

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
