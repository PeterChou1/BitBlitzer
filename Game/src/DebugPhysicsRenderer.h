#pragma once
#include <memory>

#include "Camera.h"


class DebugPhysicsRenderer
{
public:
    DebugPhysicsRenderer();

    void Update(float deltaTime);

    void Render();


private:
    std::shared_ptr<Camera> m_Cam;
    float accumulate{};
};
