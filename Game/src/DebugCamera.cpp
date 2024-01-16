#include "stdafx.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "../App/app.h"
#include "ECSManager.h"

extern ECSManager ECS;


DebugCamera::DebugCamera()
{
    m_Cam = ECS.GetResource<Camera>();
}

void DebugCamera::Move(float deltaTime)
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
        delta += m_Cam->Forward * speed;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta -= m_Cam->Forward * speed;
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

    bool button0 = App::GetController().CheckButton(XINPUT_GAMEPAD_A, false);
    bool button1 = App::GetController().CheckButton(XINPUT_GAMEPAD_B, false);
    bool button2 = App::GetController().CheckButton(XINPUT_GAMEPAD_X, false);

    std::string buttonCheck = "button 0: " + std::to_string(button0) + " "
                              "button 1: " + std::to_string(button1) + " "
                              "button 2: " + std::to_string(button2);

    App::Print(100, 150, buttonCheck.c_str());
}
