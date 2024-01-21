#include "stdafx.h"
#include "ProjectileCowCollider.h"

extern ECSManager ECS;

void ProjectileCowCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    ECS.DestroyEntity(other);
}
