#include "stdafx.h"
#include "Manifolds.h"

#include <iostream>

#include "ECSManager.h"
#include "RigidBody.h"
#include "Utils.h"
#include "Collision.h"

extern ECSManager ECS;

typedef void (*CollisionCallback) (Manifold& m, RigidBody& A, RigidBody& B);

CollisionCallback CollisionJumpTable[2][2] = {
    {Circle2Circle, Polygon2Circle},
    {Circle2Polygon, Polygon2Polygon}
};



Manifold::Manifold(Entity idA, Entity idB, RigidBody& A, RigidBody& B) : idA(idA), idB(idB), A(A), B(B)
{
    int ShapeAIdx = A.Shape.GetShapeType();
    int ShapeBIdx = B.Shape.GetShapeType();
    CollisionJumpTable[ShapeAIdx][ShapeBIdx](*this, A, B);
}

void Manifold::ResolveCollision()
{
    assert(std::abs(Normal.GetMagnitude() - 1.0) < 0.01);

    float invMassSum = A.InvMass() + B.InvMass();

    // sanity check
    if (invMassSum == 0.0)
        return;


    Vec2 rv = B.Velocity - A.Velocity;
    float vNormal = rv.Dot(Normal);

    if (vNormal < 0.0f)
        return;

    float e = (std::min)(A.Restitution(), B.Restitution());
    float j = -(1 + e) * vNormal;
    j /= invMassSum;


    Vec2 impulse = Normal * j;

    A.ApplyImpulse(impulse * -1);
    B.ApplyImpulse(impulse);
}

void Manifold::ResolveCollisionAngular()
{
    assert(std::abs(Normal.GetMagnitude() - 1.0) < 0.01);

    float e = (std::min)(A.Restitution(), B.Restitution());
    float staticFriction = (A.StaticFriction + B.StaticFriction) / 2.0f;
    float dynamicFriction = (A.DynamicFriction + B.DynamicFriction) / 2.0f;

    std::vector<Vec2> impulses;
    std::vector<float> impulseJs;
    std::vector<Vec2> contactA;
    std::vector<Vec2> contactB;

    for (int i = 0; i < ContactPoints.size(); i++)
    {

        Vec2 ra = ContactPoints[i] - A.Position;
        Vec2 rb = ContactPoints[i] - B.Position;

        Vec2 raPerp = ra.Cross(-1.0f);
        Vec2 rbPerp = rb.Cross(-1.0f);

        Vec2 angularVelocityA = raPerp * A.AngularVelocity;
        Vec2 angularVelocityB = rbPerp * B.AngularVelocity;

        Vec2 combinedVelocityA = A.Velocity + angularVelocityA;
        Vec2 combinedVelocityB = B.Velocity + angularVelocityB;

        Vec2 relativeVelocity = combinedVelocityB - combinedVelocityA;

        float vNormal = relativeVelocity.Dot(Normal);

        if (vNormal < 0.0f)
        {
            return;
        }

        float normalA = raPerp.Dot(Normal);
        float normalB = rbPerp.Dot(Normal);

        float denom = A.InvMass() + B.InvMass()
                    + (normalA * normalA) * A.InvInertia()
                    + (normalB * normalB) * B.InvInertia();


        float j = -(1.0f + e) * vNormal;

        j /= denom;
        j /= static_cast<float>(ContactPoints.size());

        Vec2 impulse = Normal * j;
        impulseJs.push_back(j);
        impulses.push_back(impulse);
        contactA.push_back(ra);
        contactB.push_back(rb);
    }

    for (int i = 0; i < ContactPoints.size(); i++)
    {
        A.ApplyImpulseAngular(impulses[i] * -1.0f, contactA[i]);
        B.ApplyImpulseAngular(impulses[i], contactB[i]);
    }
    // Calculate friction

    std::vector<Vec2> frictionImpulses;
    
    for (int i = 0; i < ContactPoints.size(); i++)
    {
    
        Vec2 ra = ContactPoints[i] - A.Position;
        Vec2 rb = ContactPoints[i] - B.Position;
    
        Vec2 raPerp = ra.Cross(-1.0f);
        Vec2 rbPerp = rb.Cross(-1.0f);
    
        Vec2 angularVelocityA = raPerp * A.AngularVelocity;
        Vec2 angularVelocityB = rbPerp * B.AngularVelocity;
    
        Vec2 combinedVelocityA = A.Velocity + angularVelocityA;
        Vec2 combinedVelocityB = B.Velocity + angularVelocityB;
    
        Vec2 relativeVelocity = combinedVelocityB - combinedVelocityA;
    
        Vec2 tangent = relativeVelocity - Normal * relativeVelocity.Dot(Normal);
    
        if (std::abs(tangent.X) <= std::numeric_limits<float>::epsilon() &&
            std::abs(tangent.Y) <= std::numeric_limits<float>::epsilon())
        {
            continue;
        }
        tangent = tangent.Normalize();
    
        float normalAT = raPerp.Dot(tangent);
        float normalBT = rbPerp.Dot(tangent);
    
        float denom = A.InvMass() + B.InvMass()
            + (normalAT * normalAT) * A.InvInertia()
            + (normalBT * normalBT) * B.InvInertia();
    
        float jt = -1.0f * relativeVelocity.Dot(tangent);
    
        jt /= denom;
        jt /= static_cast<float>(ContactPoints.size());
        // apply Coulombs law 
        Vec2 frictionImpulse{};
    
        if (std::abs(jt) <= impulseJs[i] * staticFriction)
        {
            frictionImpulse = tangent * jt;
        }
        else
        {
            frictionImpulse = tangent * -1.0f * impulseJs[i] * dynamicFriction; 
        }
        frictionImpulses.push_back(frictionImpulse);
    }
    
    for (int i = 0; i < frictionImpulses.size(); i++)
    {
        A.ApplyImpulseAngular(frictionImpulses[i], contactA[i]);
        B.ApplyImpulseAngular(frictionImpulses[i] * -1.0f, contactB[i]);
    }

}

void Manifold::PositionCorrection()
{
    const float percent = 0.8f;
    const float slop = 0.01;
    Vec2 correction = Normal * std::max(Penetration - slop, 0.0f) / (A.InvMass() + B.InvMass()) * percent;
    // Vec2 correction = Normal * (Penetration / (A.InvMass() + B.InvMass()) * percent);
    A.Position += correction * A.InvMass();
    B.Position -= correction * B.InvMass();
}