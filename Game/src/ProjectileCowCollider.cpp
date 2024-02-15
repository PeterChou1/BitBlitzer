#include "stdafx.h"
#include "ProjectileCowCollider.h"

#include "Camera.h"
#include "SpriteAnimation.h"
#include "SpriteManager.h"
#include "../App/app.h"

extern ECSManager ECS;

void ProjectileCowCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    ECS.DestroyEntity(other);
    App::PlaySound("Assets/Sounds/poofSound.wav");
    ECS.GetResource<SpriteManager>()->PlayAnimation(SmokeScreen, 0.5f, selfRB.Position.X, selfRB.Position.Y);
}
