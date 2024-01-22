#include "stdafx.h"
#include "ProjectileCowCollider.h"
#include "../App/app.h"

extern ECSManager ECS;

void ProjectileCowCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    ECS.DestroyEntity(other);
    App::PlaySound("Assets/Sounds/explosionSound.wav");
}
