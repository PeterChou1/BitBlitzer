//---------------------------------------------------------------------------------
// DebugCamera.h
//---------------------------------------------------------------------------------
//
// Basic Camera System used for debugging not for gameplay use
//
#pragma once
#include <memory>
#include "Camera.h"

class DebugCamera 
{
public:
    DebugCamera();

    void Update(float deltaTime);
    void Render();

private:
    std::shared_ptr<Camera> m_Cam;
};
