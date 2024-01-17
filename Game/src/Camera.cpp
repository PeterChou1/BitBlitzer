#include "stdafx.h"
#include "Camera.h"
#include <iostream>
#include <cassert>


void Camera::SetPosition(Vec3 camPos, Vec3 camTarget, Vec3 camUp)
{
    Position = camPos;
    Target = camTarget;
    Up = camUp;
    Backward = (Target - Position).Normalize() * -1;
    CamTransform = Transform(Position, Target, Up);
    Proj.PerspectiveOpenGL(Fov, AspectRatio, Nearplane, Farplane);
}

void Camera::UpdatePos(const Vec3& delta, const Quat& rot)
{
    Backward = rot.RotatePoint(Backward);
    Position += delta;
    Target = Position - Backward;
    CamTransform.Update(delta, rot);
}

void Camera::ToRasterSpace(Vec4& point)
{
    point.X = static_cast<int>((point.X + 1) * 0.5 * ScreenWidth);
    point.Y = static_cast<int>((point.Y + 1) * 0.5 * ScreenHeight);
    assert(0 <= point.X && point.X <= ScreenWidth);
    assert(0 <= point.Y && point.Y <= ScreenHeight);
}

void Camera::ToRasterSpaceDebug(Vec4& point)
{
    point.X = static_cast<int>((point.X + 1) * 0.5 * ScreenWidth);
    point.Y = static_cast<int>((point.Y + 1) * 0.5 * ScreenHeight);
}



Vec3 Camera::CameraToWorld(const Vec3& point)
{
    return CamTransform.Affine * point;
}

Vec3 Camera::WorldToCamera(const Vec3& point)
{
    return CamTransform.Inverse * point;
}


/**
 * \brief Helper Function for Ray Casting
 */
Vec3 RayCastPlane(Vec3 rayPoint, Vec3 rayDirection, Vec3& planePt, Vec3& planeNormal)
{
    // Calculate the dot product of the plane normal and the ray direction
    float dotProduct = planeNormal.Dot(rayDirection);

    // Check for zero (or near-zero) dot product, indicating the ray is parallel to the plane
    if (std::abs(dotProduct) < 1e-6) {
        // No intersection, or line lies within the plane
        return Vec3(0, 0, 0);
    }

    // Calculate the distance from the rayPoint to the intersection point on the plane
    float t = (planeNormal.Dot(planePt - rayPoint)) / dotProduct;

    // If t is negative, the intersection point is behind the ray's starting point
    if (t < 0) {
        return Vec3(0, 0, 0);
    }

    // Calculate the intersection point
    Vec3 intersection = rayPoint + rayDirection * t;

    return intersection;
}



Vec3 Camera::ScreenSpaceToWorldPoint(float x, float y, Vec3& planePt, Vec3& planeNormal)
{
    Vec3 right = Up.Cross(Backward).Normalize();
    const float pi = acosf(-1.0f);
    const float y_scale = tan(Fov * 0.5 * pi / 180.0) * Nearplane;
    const float x_scale = AspectRatio * y_scale;

    float clipX = x / ScreenWidth * 2.0f - 1.0f;
    float clipY = y / ScreenHeight * 2.0f - 1.0f;

    Vec3 pixelPoint = Position - Backward * Nearplane
                      + right * (x_scale * clipX)
                      + Up * (y_scale * clipY);

    Vec3 cameraRayDir = (pixelPoint - Position).Normalize();

    return RayCastPlane(Position, cameraRayDir, planePt, planeNormal);
}
