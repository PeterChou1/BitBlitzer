#include "stdafx.h"
#include "Camera.h"
#include <iostream>
#include "Utils.h"


void Camera::SetTransform(Transform t)
{
    CamTransform = t;
    Position = t.Position;
    Up = Vec3(0, 1, 0);
    Backward = (Vec3(0, 0, 0) - Position).Normalize() * -1;
}

void Camera::SetPositionAndOrientation(Vec3 camPos, Vec3 camTarget, Vec3 camUp)
{
    Position = camPos;
    Up = camUp;
    Backward = (camTarget - Position).Normalize() * -1;
    CamTransform = Transform(Position, camTarget, Up);
    Proj.PerspectiveOpenGL(Fov, AspectRatio, Nearplane, Farplane);
}

void Camera::SetPosition(Vec3 camPos)
{
    Position = camPos;
    CamTransform.SetPosition(Position);
}

void Camera::UpdatePos(const Vec3& delta, const Quat& rot)
{
    Backward = rot.RotatePoint(Backward);
    Position += delta;
    CamTransform.Update(delta, rot);
}

void Camera::ToRasterSpace(Vec4& point)
{
    point.X = static_cast<float>(static_cast<int>((point.X + 1) * 0.5 * ScreenWidth));
    point.Y = static_cast<float>(static_cast<int>((point.Y + 1) * 0.5 * ScreenHeight));
    point.X = Utils::Clamp(point.X, 0, ScreenWidth);
    point.Y = Utils::Clamp(point.Y, 0, ScreenHeight);
}

void Camera::ToRasterSpaceUnclamped(Vec4& point)
{
    point.X = static_cast<float>(static_cast<int>((point.X + 1) * 0.5 * ScreenWidth));
    point.Y = static_cast<float>(static_cast<int>((point.Y + 1) * 0.5 * ScreenHeight));
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
    if (std::abs(dotProduct) < std::numeric_limits<float>::epsilon()) {
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
    Vec3 right =  Up.Cross(Backward).Normalize() * -1.0f;
    const float y_scale = tanf(Fov * 0.5f * 3.1415926f / 180.0f) * Nearplane;
    const float x_scale = AspectRatio * y_scale;

    float clipX = x / ScreenWidth * 2.0f - 1.0f;
    float clipY = y / ScreenHeight * 2.0f - 1.0f;

    Vec3 pixelPoint = Position - Backward * Nearplane
                      + right * (x_scale * clipX)
                      + Up * (-1.0 * y_scale * clipY);

    Vec3 cameraRayDir = (pixelPoint - Position).Normalize();

    return RayCastPlane(Position, cameraRayDir, planePt, planeNormal);
}

Vec2 Camera::WorldPointToScreenSpace(Vec3 point)
{
    Vec4 projected = Proj * Vec4(CamTransform.Inverse * point);
    // perspective division
    float InverseW = 1 / projected.W;
    projected *= InverseW;
    ToRasterSpaceUnclamped(projected);
    return {projected.X, projected.Y};
}

