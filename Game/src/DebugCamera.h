#pragma once
#include "System.h"
#include "Camera.h"
#include <memory>

class DebugCamera : System
{
public:
    DebugCamera(Camera& cam) : m_cam(cam)
    {
    }

    void Move(float deltaTime);
    void Render();

private:
    Camera& m_cam;
};
