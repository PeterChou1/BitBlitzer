#include "stdafx.h"
#include "Camera.h"
#include <iostream>
#include <cassert>


void Camera::SetPosition(Vec3 camPos, Vec3 camTarget, Vec3 camUp)
{
    pos = camPos;
    target = camTarget;
    up = camUp;
    forward = (target - pos).Normalize() * -1;
    transform = Transform(pos, target, up);
    proj.PerspectiveOpenGL(fov, aspect_ratio, nearplane, farplane);
}

void Camera::UpdatePos(const Vec3& delta, const Quat& rot)
{
    forward = rot.RotatePoint(forward);
    pos += delta;
    target = pos - forward;
    transform.Update(delta, rot);
}

void Camera::ToRasterSpace(Vec4& point)
{
    point.x = static_cast<int>((point.x + 1) * 0.5 * screenWidth);
    point.y = static_cast<int>((point.y + 1) * 0.5 * screenHeight);
    assert(0 <= point.x && point.x <= screenWidth);
    assert(0 <= point.y && point.y <= screenHeight);
}

void Camera::ToRasterSpaceDebug(Vec4& point)
{
    point.x = static_cast<int>((point.x + 1) * 0.5 * screenWidth);
    point.y = static_cast<int>((point.y + 1) * 0.5 * screenHeight);
}



Vec3 Camera::CameraToWorld(const Vec3& point)
{
    return transform.Affine * point;
}

Vec3 Camera::WorldToCamera(const Vec3& point)
{
    return transform.Inverse * point;
}
