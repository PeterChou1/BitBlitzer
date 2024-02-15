#pragma once
#include "RigidBody.h"
#include "ColliderCategory.h"

class ProjectileObstaclesCollider : public Collider
{
public:

    ProjectileObstaclesCollider() : Collider(Projectile, Obstacle) {}

    ~ProjectileObstaclesCollider() override = default;

    void OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

    void OnCollideEnter(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

    void OnCollideExit(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB) override;

private:
    std::unordered_map<Entity, int> m_Collided;

};