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



Manifold::Manifold(RigidBody& A, RigidBody& B) : A(A), B(B)
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

    std::vector<Vec2> impulses;
    std::vector<Vec2> contactA;
    std::vector<Vec2> contactB;

    for (Vec2& contactPoint : ContactPoints)
    {

        Vec2 ra = contactPoint - A.Position;
        Vec2 rb = contactPoint - B.Position;

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

        impulses.push_back(impulse);
        contactA.push_back(ra);
        contactB.push_back(rb);
    }

    for (int i = 0; i < ContactPoints.size(); i++)
    {
        A.ApplyImpulseAngular(impulses[i] * -1.0f, contactA[i]);
        B.ApplyImpulseAngular(impulses[i], contactB[i]);
    }


}

void Manifold::PositionCorrection()
{
    const float percent = 1.0f;
    Vec2 correction = Normal * (Penetration / (A.InvMass() + B.InvMass()) * percent);
    A.Position += correction * A.InvMass();
    B.Position -= correction * B.InvMass();
}