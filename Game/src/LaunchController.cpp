#include "stdafx.h"

#include <iostream>

#include "LaunchController.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"
#include "Vec3.h"
#include "../App/app.h"

extern ECSManager ECS;

void LaunchController::Setup()
{
    m_Cam = ECS.GetResource<Camera>();
    m_GameState = ECS.GetResource<GameState>();
    m_LaunchRenderRadius = 200.0f;
}


void SpawnProjectile(Entity entity, Vec2 launchImpulse)
{
    auto rigidbody = RigidBody(3.5, 3.0);
    rigidbody.Color = Vec3(1.0, 0.0, 0.0);
    rigidbody.Category = Projectile;
    rigidbody.ApplyImpulse(launchImpulse);
    ECS.AddComponent<RigidBody>(entity, rigidbody);
}

void LaunchController::Update()
{
    if (m_GameState->Turn == Player1Turn)
    {
        m_LaunchPoint = Player1LaunchPoint;
    }
    else if (m_GameState->Turn == Player2Turn)
    {
        m_LaunchPoint = Player2LaunchPoint;
    }

    if (m_GameState->State == LaunchMode)
    {
        CalculateLaunchPower();
        if (m_LaunchEntity == NULL_ENTITY)
        {
            m_LaunchEntity = ECS.CreateEntity();
            Mesh mesh{};
            if (m_GameState->Turn == Player1Turn) 
                mesh = Mesh(Spot);

            if (m_GameState->Turn == Player2Turn)
                mesh = Mesh(SpotFlipped);

            ECS.AddComponent<Mesh>(m_LaunchEntity, mesh);
        }

        if (!ECS.HasComponent<Transform>(m_LaunchEntity))
        {
            ECS.AddComponent<Transform>(m_LaunchEntity, Transform());
        }
        Vec3 PlanePt = Vec3(0, 0, 0);
        Vec3 PlaneNormal = Vec3(0, 0, -1);
        Vec3 PullBack = m_Cam->ScreenSpaceToWorldPoint(m_LaunchRenderPoint.X, m_LaunchRenderPoint.Y, PlanePt, PlaneNormal);
        Vec3 TrueLaunchPoint = m_LaunchPoint - (PullBack - m_LaunchPoint) * 0.25;

        Transform& LaunchTransform = ECS.GetComponent<Transform>(m_LaunchEntity);

        Quat LaunchRotation= Quat(Vec3(0.0f, 1.0f, 0.0f), -90.0f * 3.141f / 180.0f);
        LaunchRotation *= Quat(Vec3(1.0, 0.0, 0.0), m_LaunchAngle);
        LaunchTransform = Transform(TrueLaunchPoint, LaunchRotation);
        LaunchTransform.Scale(2.0f);
        LaunchTransform.Plane = YZ;
    }


    if (m_GameState->State == LaunchMode && App::GetController().CheckButton(XINPUT_GAMEPAD_A))
    {
        assert(m_LaunchEntity != NULL_ENTITY);
        // Spawn a circle projectile
        Vec2 launchImpulse = m_LaunchDirection * m_Power * m_GameState->MaxPower;
        SpawnProjectile(m_LaunchEntity, launchImpulse);
        // Make the camera follow the projectile
        m_GameState->Projectile = m_LaunchEntity;
        m_GameState->State = FollowProjectileMode;
        m_LaunchEntity = NULL_ENTITY;
    }
}

void LaunchController::Render()
{
    if (m_GameState->State == LaunchMode)
    {
        App::DrawLine(
            m_ScreenPoint.X, m_ScreenPoint.Y,
            m_LaunchRenderPoint.X, m_LaunchRenderPoint.Y,
            1.0, 0.0, 0.0
        );
    }
}

void LaunchController::CalculateLaunchPower()
{
    float mouseX, mouseY;
    App::GetMousePos(mouseX, mouseY);
    Vec3 launchPoint = m_LaunchPoint;
    m_ScreenPoint = m_Cam->WorldPointToScreenSpace(launchPoint);
    Vec2 mousePoint = Vec2(mouseX, mouseY);
    Vec2 direction = mousePoint - m_ScreenPoint;
    Vec3 right = m_Cam->Up.Cross(m_Cam->Backward).Normalize();
    Vec2 right2D = Vec2(right.X, right.Y);

    float directionMag = direction.GetMagnitude();
    float maxDirectionMag = (m_LaunchDirection * m_LaunchRenderRadius).GetMagnitude();

    if (directionMag > maxDirectionMag)
    {
        m_Power = 1.0f;
    }
    else
    {
        m_Power = directionMag / maxDirectionMag;
    }

    m_LaunchDirection = (mousePoint - m_ScreenPoint).Normalize();

    m_LaunchAngle = Vec2::GetAngleBetween(right2D, m_LaunchDirection);
    m_LaunchRenderPoint = m_ScreenPoint + m_LaunchDirection * m_Power * maxDirectionMag;
}
