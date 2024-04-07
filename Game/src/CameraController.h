#pragma once
#include "Camera.h"
#include "GameState.h"

class CameraController
{
public:
    void Setup(Vec3 startPosition, Vec3 playePosition);

    void Update(float deltaTime);


    Vec3 Player1ViewArea{};
    Vec3 Player2ViewArea{};
    float Right{};
    float Left{};
    float Top{};
    float Bottom{};
    float ZoomOut{};
    float ZoomIn{};
    float FollowProjectileZoom{};

private:

    void HandleLerping(float deltaSeconds);

    void HandleFreeRoam(float deltaSeconds);

    void HandleStart();

    void HandleFollowProjectile();

    void LerpToTarget(float LerpTime, Vec3 target, PlayState endstate);

    void LerpToEntity(float LerpTime, Entity entity, Vec3 offset, PlayState endstate);

    // default camera speed
    Vec3 m_LerpStartPosition{};
    Vec3 m_LerpPositionTarget{};
    Vec3 m_LerpOffset{};
    Entity m_LerpTargetEntity{};
    PlayState m_LerpEndState{};
    Transform m_TransformStart{};
    Transform m_TransformPlay{};

    float m_LerpProgress{};
    float m_EntityLerpProgress{};
    float m_LerpTime = 1.0f;
    float m_Speed = 15.0f;
    std::shared_ptr<GameState> m_GameState;
    std::shared_ptr<Camera> m_Cam;
};
