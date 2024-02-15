#include "stdafx.h"
#include "CameraController.h"

#include "CowProjectile.h"
#include "ECSManager.h"
#include "Utils.h"
#include "../App/app.h"

extern ECSManager ECS;


Transform GetCamTransform(Vec3 Position)
{
    Vec3 Target = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 Up = Vec3(0.0f, 1.0, 0.0f);
    return {Position, Target, Up};
}

void CameraController::Setup(Vec3 startPosition, Vec3 playPosition)
{
    m_Cam = ECS.GetResource<Camera>();
    m_GameState = ECS.GetResource<GameState>();
    m_TransformStart = GetCamTransform(startPosition);
    m_TransformPlay = GetCamTransform(playPosition);
}

void CameraController::Update(float deltaTime)
{
    float deltaSeconds = (deltaTime / 1000);

    HandleLerping(deltaSeconds);

    if (m_GameState->Turn == Player1Win)
    {
        m_GameState->Turn = GameFinished;
        LerpToTarget(1.0, Player1ViewArea, WinnerMode);
    }

    if (m_GameState->Turn == Player2Win)
    {
        m_GameState->Turn = GameFinished;
        LerpToTarget(1.0, Player2ViewArea, WinnerMode);
    }

    if (m_GameState->Turn == Draw)
    {
        m_GameState->Turn = GameFinished;
        LerpToTarget(1.0, m_TransformPlay.Position, DrawMode);
    }

    if (m_GameState->State == StartMode || m_GameState->State == BuildMode)
    {
        HandleStart();
    }

    if (m_GameState->State == FreeRoamMode)
    {
        HandleFreeRoam(deltaSeconds);
    }

    if (m_GameState->State == FollowProjectileMode)
    {
        HandleFollowProjectile();
    }
}

void CameraController::HandleLerping(float deltaSeconds)
{
    if (m_GameState->State == IntroMode)
    {
        m_LerpProgress += deltaSeconds / 5.0f;
        m_LerpProgress = Utils::Clamp(m_LerpProgress, 0, 1);
        Transform camTransform = Transform::Lerp(m_TransformStart, m_TransformPlay, m_LerpProgress);
        m_Cam->SetTransform(camTransform);
        if (m_LerpProgress == 1.0) m_GameState->State = StartMode;
    }

    if (m_GameState->State == CameraLerpToPosition)
    {
        m_LerpProgress += deltaSeconds / m_LerpTime;
        m_LerpProgress = Utils::Clamp(m_LerpProgress, 0, 1);
        Vec3 newPosition = Vec3::Lerp(m_LerpStartPosition, m_LerpPositionTarget, m_LerpProgress);
        m_Cam->SetPosition(newPosition);
        if (m_LerpProgress == 1.0) m_GameState->State = m_LerpEndState;
    }

    if (m_GameState->State == CameraLerpToEntity)
    {
        assert(m_LerpTargetEntity != NULL_ENTITY);
        if (!ECS.HasComponent<Transform>(m_LerpTargetEntity)) 
        {
            m_GameState->State = m_LerpEndState;
            return;
        }
        m_EntityLerpProgress += deltaSeconds / m_LerpTime;
        m_EntityLerpProgress = Utils::Clamp(m_EntityLerpProgress, 0, 1);
        Vec3 target = ECS.GetComponent<Transform>(m_LerpTargetEntity).Position + m_LerpOffset;
        Vec3 newPosition = Vec3::Lerp(m_LerpStartPosition, target, m_EntityLerpProgress);
        m_Cam->SetPosition(newPosition);
        if (m_EntityLerpProgress == 1.0) m_GameState->State = m_LerpEndState;
    }
}

void CameraController::HandleFreeRoam(float deltaSeconds)
{
    Vec3 delta = Vec3(0, 0, 0);
    // scroll the camera faster the more you zoom
    float zoomLevel = m_Cam->Position.Z / 5.0f;

    float speed = m_Speed * deltaSeconds;

    Vec3 right = m_Cam->Up.Cross(m_Cam->Backward).Normalize();

    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        delta += right * speed * zoomLevel;
    }
    if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        delta -= right * speed * zoomLevel;
    }

    if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        delta -= m_Cam->Up * speed * zoomLevel;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta += m_Cam->Up * speed * zoomLevel;
    }

    if (App::IsKeyPressed('Q'))
    {
        delta += m_Cam->Backward * speed;
    }

    if (App::IsKeyPressed('E'))
    {
        delta -= m_Cam->Backward * speed;
    }
    Vec3 NewPosition = m_Cam->Position + delta;

    NewPosition.X = Utils::Clamp(NewPosition.X, Left, Right);
    NewPosition.Y = Utils::Clamp(NewPosition.Y, Bottom, Top);
    NewPosition.Z = Utils::Clamp(NewPosition.Z, ZoomIn, ZoomOut);

    m_Cam->SetPosition(NewPosition);

    if (App::GetController().CheckButton(XINPUT_GAMEPAD_A))
    {
        Vec3 lerpTarget;

        if (m_GameState->Turn == Player1Turn)
            lerpTarget = Player1ViewArea;

        if (m_GameState->Turn == Player2Turn)
            lerpTarget = Player2ViewArea;

        const float differenceMag = (lerpTarget - m_Cam->Position).GetMagnitude();

        if (differenceMag < 5.0f)
        {
            LerpToTarget(0.1f, lerpTarget, LaunchMode);
            return;
        }
        LerpToTarget(1.0f, lerpTarget, LaunchMode);
    }
}

void CameraController::HandleStart()
{
    if (m_GameState->State == StartMode)
    {
        LerpToTarget(1.0, Player1ViewArea, BuildMode);
    }

    if (App::GetController().CheckButton(XINPUT_GAMEPAD_A) && m_GameState->Turn == Player1Turn)
    {
        LerpToTarget(1.0, Player2ViewArea, BuildMode);
        m_GameState->Turn = Player2Turn;

    }
    else if (App::GetController().CheckButton(XINPUT_GAMEPAD_A) && m_GameState->Turn == Player2Turn)
    {

        int player1Projectile = m_GameState->Player1Inventory.regularCow + m_GameState->Player1Inventory.explosiveCow;

        if (player1Projectile > 0)
        {
            LerpToTarget(1.0, Player1ViewArea, FreeRoamMode);
            m_GameState->Turn = Player1Turn;
        }
        else
        {
            LerpToTarget(1.0, Player2ViewArea, FreeRoamMode);
            m_GameState->Turn = Player2Turn;
        }
    }
}

void CameraController::HandleFollowProjectile()
{
    Entity projectile = m_GameState->Projectile;
    assert(projectile != NULL_ENTITY);
    std::set<Entity> deletedSet = ECS.VisitDeleted<Transform>();

    if (deletedSet.find(projectile) != deletedSet.end())
    {

        int Player1ProjectileCount = m_GameState->Player1Inventory.regularCow + m_GameState->Player1Inventory.explosiveCow;
        int Player2ProjectileCount = m_GameState->Player2Inventory.regularCow + m_GameState->Player2Inventory.explosiveCow;

        // Projectile Deleted Switch Player Turn
        if (m_GameState->Turn == Player1Turn && Player2ProjectileCount > 0)
        {
            m_GameState->Turn = Player2Turn;
            LerpToTarget(1.0, Player2ViewArea, FreeRoamMode);
        }
        else if (m_GameState->Turn == Player2Turn && Player1ProjectileCount > 0)
        {
            m_GameState->Turn = Player1Turn;
            LerpToTarget(1.0, Player1ViewArea, FreeRoamMode);
        }
        else if (m_GameState->Turn == Player2Turn)
        {
            LerpToTarget(1.0, Player2ViewArea, FreeRoamMode);
        }
        else if (m_GameState->Turn == Player1Turn)
        {
            LerpToTarget(1.0, Player1ViewArea, FreeRoamMode);
        }

        return;
    }

    Transform& transform = ECS.GetComponent<Transform>(projectile);
    Vec3 offset = Vec3(0.0, 0.0, FollowProjectileZoom);
    Vec3 target = transform.Position + offset;
    float differenceMag = (target - m_Cam->Position).GetMagnitude();

    if (differenceMag < 5.0f)
    {
        m_Cam->SetPosition(target);
    }
    else
    {
        LerpToEntity(1.0, projectile, offset, FollowProjectileMode);
    }
}

void CameraController::LerpToTarget(float LerpTime, Vec3 target, PlayState endstate)
{
    m_GameState->State = CameraLerpToPosition;
    m_LerpStartPosition = m_Cam->Position;
    m_LerpPositionTarget = target;
    m_LerpProgress = 0;
    m_LerpEndState = endstate;
    m_LerpTime = LerpTime;
}

void CameraController::LerpToEntity(float LerpTime, Entity entity, Vec3 offset, PlayState endstate)
{
    assert(ECS.HasComponent<Transform>(entity));
    m_GameState->State = CameraLerpToEntity;
    m_LerpStartPosition = m_Cam->Position;
    m_EntityLerpProgress = 0;
    m_LerpOffset = offset;
    m_LerpEndState = endstate;
    m_LerpTargetEntity = entity;
    m_LerpTime = LerpTime;
}
