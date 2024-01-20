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
    m_LaunchRenderRadius = 100.0f;
}


void SpawnProjectile(Vec3 launchPoint, Vec2 launchImpulse)
{
    Entity entity = ECS.CreateEntity();
    float radians = -90.0f * 3.141 / 180.0f;
    auto transform = Transform(launchPoint, Quat(Vec3(0.0f, 1.0f, 0.0f), radians));
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(2.0, 2.0);
    rigidbody.Color = Vec3(1.0, 0.0, 0.0);
    rigidbody.Category = Projectile;
    rigidbody.ApplyImpulse(launchImpulse);
    ECS.AddComponent<RigidBody>(entity, rigidbody);
    ECS.AddComponent<Mesh>(entity, Mesh(Spot));
}

void LaunchController::Update(float deltaTime)
{
    m_Accumulate += deltaTime / 1000.0f;

    if (m_Accumulate > 0.1 && App::GetController().CheckButton(XINPUT_GAMEPAD_A))
    {
        // Spawn a circle projectile
        Vec2 launchImpulse = m_LaunchDirection * m_Power * m_GameState->MaxPower;
        SpawnProjectile(m_GameState->LaunchPoint, launchImpulse);
    }
}

void LaunchController::Render()
{
    float mouseX, mouseY;
    App::GetMousePos(mouseX, mouseY);
    Vec3 launchPoint = m_GameState->LaunchPoint;
    Vec2 screenPoint = m_Cam->WorldPointToScreenSpace(launchPoint);
    Vec2 mousePoint = Vec2(mouseX, mouseY);
    Vec2 direction = mousePoint - screenPoint;

    m_LaunchDirection = (mousePoint - screenPoint).Normalize();
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

    Vec2 directionPoint = screenPoint + m_LaunchDirection * m_Power * maxDirectionMag;

    App::DrawLine(
        screenPoint.X, screenPoint.Y, 
        directionPoint.X, directionPoint.Y,
        1.0, 0.0, 0.0
    );


}
