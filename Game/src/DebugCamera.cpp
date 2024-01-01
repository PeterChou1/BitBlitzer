#include "stdafx.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "../App/app.h"
#include <iostream>
#include <sstream>

extern Coordinator gCoordinator;


void DebugCamera::Move(float deltaTime)
{
    Camera& cam = GetFirstComponent<Camera>(gCoordinator);

    Vec3& delta = Vec3(0, 0, 0);
    const Vec3& target = cam.forward;
    Quat r = Quat(cam.up, 0);

    float speed = 5 * (deltaTime / 1000);
    float rotation = 3.141 / 2 * (deltaTime / 1000);
    // 
    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        r *= Quat(cam.up, -rotation);
    }
    if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        r *= Quat(cam.up, rotation);
    }

    if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        delta += cam.forward * speed;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta -= cam.forward * speed;
    }

    if (App::IsKeyPressed('Q'))
    {
        delta += cam.up * speed;
    }

    if (App::IsKeyPressed('E'))
    {
        delta -= cam.up * speed;
    }

    cam.UpdatePos(delta, r);
}

void DebugCamera::Render()
{
    Camera& cam = GetFirstComponent<Camera>(gCoordinator);
    std::string pos = "Position x:" + std::to_string(cam.pos.x) + " y: " + std::to_string(cam.pos.y) + " z: " +
        std::to_string(cam.pos.z);
    App::Print(100, 100, pos.c_str());
}
