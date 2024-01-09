#include "stdafx.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "../App/app.h"
#include "ECSManager.h"

extern ECSManager ECS;


DebugCamera::DebugCamera()
{
    m_cam = ECS.GetResource<Camera>();
}

void DebugCamera::Move(float deltaTime)
{
    Vec3 delta = Vec3(0, 0, 0);
    Quat r = Quat(m_cam->up, 0);

    float speed = 5 * (deltaTime / 1000);
    float rotation = 3.141f / 2.0f * (deltaTime / 1000.0f);
    // 
    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        r *= Quat(m_cam->up, -rotation);
    }
    if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        r *= Quat(m_cam->up, rotation);
    }

    if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        delta += m_cam->forward * speed;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta -= m_cam->forward * speed;
    }

    if (App::IsKeyPressed('Q'))
    {
        delta += m_cam->up * speed;
    }

    if (App::IsKeyPressed('E'))
    {
        delta -= m_cam->up * speed;
    }

    m_cam->UpdatePos(delta, r);
}

void DebugCamera::Render()
{
    std::string pos = "Position x:" + std::to_string(m_cam->pos.x) + " y: " + std::to_string(m_cam->pos.y) + " z: " + std::to_string(m_cam->pos.z);
    App::Print(100, 100, pos.c_str());
}
