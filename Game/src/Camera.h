#pragma once
#include "MeshInstance.h"
#include "Mat4.h"
#include "Resource.h"
#include "../App/AppSettings.h"

class Camera : public Resource
{
public:
    Mat4 proj{};
    Transform transform{};
    Vec3 pos{};
    Vec3 target{};
    Vec3 forward{};
    Vec3 up{};
    // default camera values
    float nearplane = 0.1f;
    float farplane = 100.0f;
    float fov = 90.0;
    float screenHeight = APP_VIRTUAL_HEIGHT;
    float screenWidth = APP_VIRTUAL_WIDTH;
    float aspect_ratio = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


    Camera() = default;

    void SetPosition(Vec3 camPos, Vec3 camTarget, Vec3 camUp);

    void UpdatePos(const Vec3& delta, const Quat& rot);

    void ToRasterSpace(Vec4& point);

    void ResetResource() override {}

    Vec3 CameraToWorld(const Vec3& point);

    Vec3 WorldToCamera(const Vec3& point);

};
