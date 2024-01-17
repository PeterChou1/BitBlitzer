//---------------------------------------------------------------------------------
// Camera.h
//---------------------------------------------------------------------------------
//
// The Camera controls the viewpoint of the player in the game
//
#pragma once

#include "MeshInstance.h"
#include "Mat4.h"
#include "Resource.h"
#include "../App/AppSettings.h"

class Camera : public Resource
{
public:
    // Camera Projection Matrix used during the rendering process
    Mat4 Proj{};
    // Camera Transform specifies controls
    // Camera Position and Orientation
    Transform CamTransform{};
    Vec3 Position{};
    // Vector indicating where camera is pointing
    Vec3 Target{};
    // Backward Vector pointing away from where the camera is pointing
    Vec3 Backward{};
    // Up Vector pointing up in the camera current orientation
    Vec3 Up{};
    // -- Default camera values used to construct OpenGL matrix --
    float Nearplane = 0.1f;
    float Farplane = 100.0f;
    float Fov = 90.0;
    float ScreenHeight = APP_VIRTUAL_HEIGHT;
    float ScreenWidth = APP_VIRTUAL_WIDTH;
    float AspectRatio = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

    Camera() = default;

    /**
     * \brief Reset camera position and orientation
     * \param camPos Vector to set camera position
     * \param camTarget Vector to set camera target 
     * \param camUp Vector to set up Vector
     */
    void SetPosition(Vec3 camPos, Vec3 camTarget, Vec3 camUp);

    /**
     * \brief Updates Camera by position by delta and rotation by rot
     * \param delta vector to add to camera position
     * \param rot quaternion to add to camera rotation
     */
    void UpdatePos(const Vec3& delta, const Quat& rot);

    /**
     * \brief Translate Clip Space Point to Raster Space used in the
     *        Render pipeline during clipping
     */
    void ToRasterSpace(Vec4& point);

    /**
     * \brief A Debug Version of ToRasterSpace without the safety
     *        Checks used by Debugging systems not intended for
     *        gameplay use
     */
    void ToRasterSpaceDebug(Vec4& point);

    /**
     * \brief A Camera is a resources but resetting it does nothing
     */
    void ResetResource() override
    {
    }

    /**
     * \brief Translates a point from camera space to world space
     */
    Vec3 CameraToWorld(const Vec3& point);

    /**
     * \brief Translates a point from world space to camera space
     */
    Vec3 WorldToCamera(const Vec3& point);

    /**
     * \brief Translates a mouse position to a world space plane this
     *        method essentially raycast from a camera to a point
     *        if the raycast fails it will return zero vector
     * \param x Mouse Position X
     * \param y Mouse Position Y
     * \param planePt A point on the plane you want to raycast to
     * \param planeNormal plane normal of the plane
     * \return point on the plane
     */
    Vec3 ScreenSpaceToWorldPoint(float x, float y, Vec3& planePt, Vec3& planeNormal);
};
