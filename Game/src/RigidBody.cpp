#include "stdafx.h"

#include <cassert>

#include "RigidBody.h"


RigidBody::RigidBody(float radius)
{
    Shape = Shape::CreateCircle(radius);
    RigidBodyAABB = AABB(radius);
    float area = radius * radius * 3.141;
    float mass = DEFAULT_DENSITY * area;
    float inertia = 0.5f * mass * radius * radius;
    m_InvMass = 1.0f / mass;
    m_InvInertia = 1.0f / inertia;
    m_Restitution = DEFAULT_RESTITUTION;
    StaticFriction = DEFAULT_STATIC_FRICTION;
    DynamicFriction = DEFAULT_DYNAMIC_FRICTION;
}

RigidBody::RigidBody(float width, float height)
{
    Shape = Shape::CreateRect(width, height);
    RigidBodyAABB = AABB(Shape.PolygonPoints);
    float area = width * height;
    float mass = area * DEFAULT_DENSITY;
    float inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
    m_InvMass = 1.0f / mass;
    m_InvInertia = 1.0f / inertia;
    m_Restitution = DEFAULT_RESTITUTION;
    StaticFriction = DEFAULT_STATIC_FRICTION;
    DynamicFriction = DEFAULT_DYNAMIC_FRICTION;
}

RigidBody::RigidBody(std::vector<Vec2> polygons)
{
    assert(false && "not implemented");
}

void RigidBody::SetStatic()
{
    m_InvMass = 0.0f;
    m_InvInertia = 0.0f;
}

void RigidBody::SyncTransform(Transform& transform, SlicePlane plane)
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

void RigidBody::ForwardTransform(Transform& transform, SlicePlane plane) const
{
    transform.SetPosition2D(Position, plane);
    switch (plane)
    {
    case YZ:
        transform.UpdateYaw(AngularDelta);
        break;
    case XZ:
        transform.UpdatePitch(AngularDelta);
        break;
    case XY:
        transform.UpdateRow(AngularDelta);
    }
}

void RigidBody::RecomputeAABB()
{
    RigidBodyAABB.RecomputeAABB(Position, Angular, Shape.GetShapeType());
}

void RigidBody::ApplyImpulse(const Vec2& impulse)
{
    Velocity += impulse * m_InvMass;
}

void RigidBody::ApplyImpulseAngular(const Vec2& impulse, const Vec2& contactVector)
{
    Velocity += impulse * m_InvMass;
    AngularVelocity += contactVector.Cross(impulse) * m_InvInertia;
}

void RigidBody::IntegrateVelocity(float deltaTime)
{
    if (m_InvMass == 0.0f) return;
    Position += Velocity * deltaTime;
}

void RigidBody::IntegrateVelocityAngular(float deltaTime)
{
    if (m_InvMass == 0.0f) return;
    Position += Velocity * deltaTime;
    AngularDelta = AngularVelocity * deltaTime;
    Angular += AngularDelta;
}

float RigidBody::InvMass() const
{
    return m_InvMass;
}

float RigidBody::InvInertia() const
{
    return m_InvInertia;
}

float RigidBody::Restitution() const
{
    return m_Restitution;
}
