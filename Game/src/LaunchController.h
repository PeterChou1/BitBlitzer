#pragma once
#include "Camera.h"
#include "GameState.h"

class LaunchController
{
public:

    void Setup();

    void Update();

    void Render();

    Vec3 Player1LaunchPoint{};
    Vec3 Player2LaunchPoint{};

private:

    void CalculateLaunchPower();

    Entity m_LaunchEntity{};
    Vec2 m_ScreenPoint{};
    Vec2 m_LaunchRenderPoint{};

    Vec3 m_LaunchPoint{};
    float m_Power{};
    float m_LaunchRenderRadius{};
    float m_LaunchAngle{};
    Vec2 m_LaunchDirection{};
    std::shared_ptr<GameState> m_GameState;
    std::shared_ptr<Camera> m_Cam;
};
