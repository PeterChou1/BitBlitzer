#include "stdafx.h"
#include "CameraController.h"
#include "ECSManager.h"
#include "../App/app.h"

extern ECSManager ECS;

void CameraController::Setup(Vec3 position)
{
    m_Cam = ECS.GetResource<Camera>();
    Vec3 up = Vec3(0.0f, 1.0, 0.0f);
    // give camera angle a slight tilt to give it a more 3d feeling
    // float radians = -45.0f * 3.141 / 180.0f;
    // Quat tilt = Quat(Vec3(1.0f, 0.0f, 0.0f), radians);
    // up = tilt.RotatePoint(up);

    m_Cam->SetPosition(position,Vec3(0.0f, 0.0f, 0.0f), up);
}

void CameraController::Update(float deltaTime)
{
    Vec3 delta = Vec3(0, 0, 0);
    float speed = m_Speed * (deltaTime / 1000);
    Vec3 right = m_Cam->Up.Cross(m_Cam->Backward).Normalize();

    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        delta += right * speed;
    }
    if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        delta -= right * speed;
    }

    if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        delta -= m_Cam->Up * speed;
    }
    if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        delta += m_Cam->Up * speed;
    }

    if (App::IsKeyPressed('Q'))
    {
        delta += m_Cam->Backward * speed;

    }

    if (App::IsKeyPressed('E'))
    {
        delta -= m_Cam->Backward * speed;
    }

    m_Cam->UpdatePos(delta, Quat());
}
