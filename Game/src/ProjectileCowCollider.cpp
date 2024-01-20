#include "stdafx.h"
#include "ProjectileCowCollider.h"

#include "Mesh.h"

extern ECSManager ECS;

void ProjectileCowCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    ECS.DestroyEntity(other);

    Vec3 Pos = ECS.GetComponent<Transform>(self).Position;

    if (4.9 <= Pos.X && Pos.X <= 5.5)
    {
        std::cout << "On Collide Spot: " + ECS.GetComponent<Transform>(self).Position.ToString() + "\n";
    }
}
