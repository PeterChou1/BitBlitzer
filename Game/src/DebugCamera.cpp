#include "stdafx.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "../App/app.h"
#include "ECSManager.h"
#include "RenderConstants.h"

extern ECSManager ECS;



DebugCamera::DebugCamera()
{
    m_Cam = ECS.GetResource<Camera>();
}

void DebugCamera::Update(float deltaTime)
{
    Vec3 delta = Vec3(0, 0, 0);
    Quat r = Quat(m_Cam->Up, 0);

    float speed = 5 * (deltaTime / 1000);
    float rotation = 3.141f / 2.0f * (deltaTime / 1000.0f);
    // 
    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        r *= Quat(m_Cam->Up, -rotation);
    }
    if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        r *= Quat(m_Cam->Up, rotation);
    }

    if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        delta += m_Cam->Backward * speed;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta -= m_Cam->Backward * speed;
    }

    if (App::IsKeyPressed('Q'))
    {
        delta += m_Cam->Up * speed;
    }

    if (App::IsKeyPressed('E'))
    {
        delta -= m_Cam->Up * speed;
    }
    m_Cam->UpdatePos(delta, r);
}

void DebugCamera::Render()
{
    std::string pos = "Position x:" + std::to_string(m_Cam->Position.X) + " y: " + std::to_string(m_Cam->Position.Y) + " z: " + std::to_string(m_Cam->Position.Z);
    App::Print(100, 100, pos.c_str());
}
