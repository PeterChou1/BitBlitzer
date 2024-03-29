#pragma once
#include "RigidBody.h"
#include "ColliderCategory.h"

class ProjectileCowCollider : public Collider
{
public:

    ProjectileCowCollider() : Collider(Projectile, Cow) {}

    ~ProjectileCowCollider() override = default;

    void OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

    void OnCollideEnter(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override {}

    void OnCollideExit(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override {}
};
