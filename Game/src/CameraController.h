#pragma once
#include "Camera.h"

class CameraController
{
public:
    void Setup(Vec3 position);

    void Update(float deltaTime);

private:
    // default camera speed
    float m_Speed = 10.0f;
    std::shared_ptr<Camera> m_Cam;
};
