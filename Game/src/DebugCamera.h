#pragma once
#include <memory>

#include "Camera.h"

class DebugCamera 
{
public:
    DebugCamera();

    void Move(float deltaTime);
    void Render();

private:
    std::shared_ptr<Camera> m_cam;
};
