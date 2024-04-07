#include "stdafx.h"
#include "ProjectileKillBoxCollider.h"

extern ECSManager ECS;

void ProjectileKillBoxCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    ECS.DestroyEntity(self);
}
