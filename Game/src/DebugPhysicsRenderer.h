//---------------------------------------------------------------------------------
// DebugPhysicsRenderer.h
//---------------------------------------------------------------------------------
//
// Basic debug System used to render physics object not for gameplay use
//
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
