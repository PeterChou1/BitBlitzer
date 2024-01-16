#pragma once
#include "MeshInstance.h"
#include "Mat4.h"
#include "Resource.h"
#include "../App/AppSettings.h"

class Camera : public Resource
{
public:
    Mat4 Proj{};
    Transform CamTransform{};
    Vec3 Position{};
    Vec3 Target{};
    Vec3 Forward{};
    Vec3 Up{};
    // default camera values
    float Nearplane = 0.1f;
    float Farplane = 100.0f;
    float Fov = 90.0;
    float ScreenHeight = APP_VIRTUAL_HEIGHT;
    float ScreenWidth = APP_VIRTUAL_WIDTH;
    float AspectRatio = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;


    Camera() = default;

    void SetPosition(Vec3 camPos, Vec3 camTarget, Vec3 camUp);

    void UpdatePos(const Vec3& delta, const Quat& rot);

    void ToRasterSpace(Vec4& point);

    // To Raster Space but without safety checks
    void ToRasterSpaceDebug(Vec4& point);

    void ResetResource() override
    {
    }

    Vec3 CameraToWorld(const Vec3& point);

    Vec3 WorldToCamera(const Vec3& point);

    Vec3 RasterSpaceToWorldPoint(float x, float y, Vec3& planePt, Vec3& planeNormal);
};
