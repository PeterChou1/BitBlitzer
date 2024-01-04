#pragma once
#include "Camera.h"

class DebugCamera 
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
