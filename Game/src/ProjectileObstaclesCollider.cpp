#include "stdafx.h"
#include "ProjectileObstaclesCollider.h"

#include "Timer.h"

extern ECSManager ECS;

void ProjectileObstaclesCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    if (!ECS.HasComponent<Timer>(self))
    {
        // Give projectile 3 seconds after hitting a obstacle to delete itself
        ECS.AddComponent<Timer>(self, Timer(5.0f));
    }
}
