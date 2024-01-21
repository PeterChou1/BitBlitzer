#pragma once
#include "RigidBody.h"
#include "ColliderCategory.h"

class ProjectileKillBoxCollider : public Collider
{
public:

    ProjectileKillBoxCollider() : Collider(Projectile, KillBox) {}

    ~ProjectileKillBoxCollider() override = default;

    void OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

};