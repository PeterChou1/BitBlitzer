#pragma once
#include "Mesh.h"
#include "Mat4.h"


class Camera
{
public:
    Mat4 proj;
    Transform transform;
    Vec3 pos;
    Vec3 target;
    Vec3 forward;
    Vec3 up;
    float nearplane;
    float farplane;
    float screenHeight;
    float screenWidth;


    Camera()
    {
    };

    Camera(Vec3& pos,
           Vec3& target,
           Vec3& up,
           float height, float width,
           float fov, float aspect_ratio,
           float nearplane, float farplane);

    void UpdatePos(const Vec3& pos, const Quat& rot);

    void ToRasterSpace(Vec4& point);

    Vec3 CameraToWorld(const Vec3& point);

    Vec3 WorldToCamera(const Vec3& point);
};
