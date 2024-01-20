#pragma once
#include "Camera.h"
#include "GameState.h"

class LaunchController
{
public:
    void Setup();

    void Update(float deltaTime);

    void Render();

private:
    float m_Accumulate{};
    float m_Power{};
    float m_LaunchRenderRadius{};
    Vec2 m_LaunchDirection{};
    std::shared_ptr<GameState> m_GameState;
    std::shared_ptr<Camera> m_Cam;
};
