#pragma once
#include <memory>

#include "Camera.h"


class DebugPhysicsRenderer
{
public:
    DebugPhysicsRenderer();

    void Render();
private:
    std::shared_ptr<Camera> m_cam;

};
