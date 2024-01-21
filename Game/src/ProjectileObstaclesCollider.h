#pragma once
#include "RigidBody.h"
#include "ColliderCategory.h"

class ProjectileObstaclesCollider : public Collider
{
public:

    ProjectileObstaclesCollider() : Collider(Projectile, Obstacle) {}

    ~ProjectileObstaclesCollider() override = default;

    void OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

};