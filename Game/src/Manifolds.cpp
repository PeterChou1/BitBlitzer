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



Manifold::Manifold(Entity A, Entity B) : A(A), B(B)
{
    assert(ECS.HasComponent<RigidBody>(A) && ECS.HasComponent<RigidBody>(B));
    RigidBody& ABody = ECS.GetComponent<RigidBody>(A);
    RigidBody& BBody = ECS.GetComponent<RigidBody>(B);
    int ShapeAIdx = ABody.Shape.GetShapeType();
    int ShapeBIdx = BBody.Shape.GetShapeType();
    CollisionJumpTable[ShapeAIdx][ShapeBIdx](*this, ABody, BBody);
}

void Manifold::ResolveCollision()
{
    assert(std::abs(Normal.GetMagnitude() - 1.0) < 0.01);

    RigidBody& ABody = ECS.GetComponent<RigidBody>(A);
    RigidBody& BBody = ECS.GetComponent<RigidBody>(B);

    float invMassSum = ABody.InvMass() + BBody.InvMass();

    // sanity check
    if (invMassSum == 0.0)
        return;


    Vec2 rv = BBody.Velocity - ABody.Velocity;
    float vNormal = rv.Dot(Normal);

    if (vNormal < 0.0f)
        return;

    float e = (std::min)(ABody.Restitution(), BBody.Restitution());
    float j = -(1 + e) * vNormal;
    j /= invMassSum;


    Vec2 impulse = Normal * j;

    ABody.ApplyImpulse(impulse * -1);
    BBody.ApplyImpulse(impulse);
}

void Manifold::ResolveCollisionAngular()
{
    assert(std::abs(Normal.GetMagnitude() - 1.0) < 0.01);

    RigidBody& ABody = ECS.GetComponent<RigidBody>(A);
    RigidBody& BBody = ECS.GetComponent<RigidBody>(B);

    float e = (std::min)(ABody.Restitution(), BBody.Restitution());

    std::vector<Vec2> impulses;
    std::vector<Vec2> contactA;
    std::vector<Vec2> contactB;

    for (Vec2& contactPoint : ContactPoints)
    {

        Vec2 ra = contactPoint - ABody.Position;
        Vec2 rb = contactPoint - BBody.Position;

        Vec2 raPerp = ra.Cross(-1.0f);
        Vec2 rbPerp = rb.Cross(-1.0f);

        Vec2 angularVelocityA = raPerp * ABody.AngularVelocity;
        Vec2 angularVelocityB = rbPerp * BBody.AngularVelocity;

        Vec2 combinedVelocityA = ABody.Velocity + angularVelocityA;
        Vec2 combinedVelocityB = BBody.Velocity + angularVelocityB;

        Vec2 relativeVelocity = combinedVelocityB - combinedVelocityA;

        float vNormal = relativeVelocity.Dot(Normal);

        if (vNormal < 0.0f)
        {
            return;
        }

        float normalA = raPerp.Dot(Normal);
        float normalB = rbPerp.Dot(Normal);

        float denom = ABody.InvMass() + BBody.InvMass()
                    + (normalA * normalA) * ABody.InvInertia()
                    + (normalB * normalB) * BBody.InvInertia();


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
        ABody.ApplyImpulseAngular(impulses[i] * -1.0f, contactA[i]);
        BBody.ApplyImpulseAngular(impulses[i], contactB[i]);
    }


}

void Manifold::PositionCorrection()
{
    RigidBody& ABody = ECS.GetComponent<RigidBody>(A);
    RigidBody& BBody = ECS.GetComponent<RigidBody>(B);
    const float percent = 0.8f;
    Vec2 correction = Normal * (Penetration / (ABody.InvMass() + BBody.InvMass()) * percent);
    ABody.Position += correction * ABody.InvMass();
    BBody.Position -= correction * BBody.InvMass();
}