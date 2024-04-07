#include "stdafx.h"

#include "ProjectileObstaclesCollider.h"
#include "CowProjectile.h"
#include "SpriteAnimation.h"
#include "SpriteManager.h"
#include "Timer.h"
#include "app.h"

extern ECSManager ECS;

void ProjectileObstaclesCollider::OnCollide(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    bool hasProjectile = ECS.HasComponent<CowProjectile>(self);
    if (hasProjectile)
    {
        CowProjectile projectile = ECS.GetComponent<CowProjectile>(self);
        if (projectile.ProjectileType == ExplosiveCow)
        {
            ECS.GetResource<SpriteManager>()->PlayAnimation(Explosion, 6.0f, selfRB.Position.X, selfRB.Position.Y);
            // App::PlaySound("Assets/Sounds/explosionSound.wav");
            AABB destroyBox = selfRB.RigidBodyAABB;
            destroyBox.Max += Vec2(3.0, 3.0);
            destroyBox.Min -= Vec2(3.0, 3.0);
            // Destroy all rigidbodies within the AABB
            std::vector<Entity> destroyList;

            for (auto& entity : ECS.Visit<RigidBody>())
            {
                if (entity == self) continue;
                RigidBody& rb = ECS.GetComponent<RigidBody>(entity);
                if (AABBTest(destroyBox, rb.RigidBodyAABB) && rb.Collidable && rb.InvMass() > 0.0f)
                {
                    destroyList.push_back(entity);
                }
            }
            ECS.DestroyEntity(self);
            for (auto& entity : destroyList)
            {
                ECS.DestroyEntity(entity);
            }
        }
        else if (!ECS.HasComponent<Timer>(self))
        {
            // Give projectile 5 seconds after hitting a obstacle to delete itself
            ECS.AddComponent<Timer>(self, Timer(5.0f));
        }
    }
}

void ProjectileObstaclesCollider::OnCollideEnter(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    if (m_Collided.find(self) != m_Collided.end())
    {
        if (m_Collided[self] == 0 && !App::IsSoundPlaying("Assets/Sounds/bounceSound.wav"))
        {
            App::PlaySound("Assets/Sounds/bounceSound.wav");
        }
        m_Collided[self] += 1;
    }
    else
    {
        m_Collided[self] = 1;
        if (!App::IsSoundPlaying("Assets/Sounds/bounceSound.wav"))
        {
            App::PlaySound("Assets/Sounds/bounceSound.wav");
        }
    }
}

void ProjectileObstaclesCollider::OnCollideExit(Entity self, Entity other, RigidBody& selfRB, RigidBody& otherRB)
{
    if (m_Collided.find(self) != m_Collided.end())
    {
        m_Collided[self] -= 1;
    }
}
