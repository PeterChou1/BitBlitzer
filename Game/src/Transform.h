//---------------------------------------------------------------------------------
// Transform.h
//---------------------------------------------------------------------------------
// 
// Transform are a way to represent location and orientation within the engine
// any objects located in the world space will have a transform attach to them in
// the Entity Component System
//
#pragma once

#include "Vec3.h"
#include "Quat.h"
#include "Mat4.h"


/**
 * \brief Since the transform class is 3d we have three ways
 *        to slice the 3d coordinates to sync up with our 2d
 *        physics system
 */
enum SlicePlane
{
    XY,
    XZ,
    YZ
};


struct Transform
{
    Vec3 Position;
    Quat Rotation;
    float TransformScale;
    Mat4 Affine;
    Mat4 Inverse;
    SlicePlane Plane = XY;
    bool IsDirty{};

    /**
     * \brief Default Constructor Transform will be initialized at (0,0,0)
     */
    Transform();

    /**
     * \brief Construct a transform given an affine matrix
     *        Rotation and position are obtain from the affine matrix
     * \param affine
     */
    Transform(const Mat4& affine);

    /**
     * \brief Construct a transform given only a position
     *        The object will be rotated in its default position
     * \param pos 
     */
    Transform(const Vec3& pos);

    /**
     * \brief Construct a transform based on position and quaternion
     *        Rotation
     * \param pos 
     * \param rot 
     */
    Transform(const Vec3& pos, const Quat& rot);


    /**
     * \brief Scale transform uniformly in all direction
     * \param scale 
     */
    void Scale(float scale);

    /**
     * \brief Constructor a transform based on a Camera oriented approach
     *        where we specify a target vector to point the transform to  
     * \param pos
     * \param target vector of where the transform is pointing to
     * \param up vector indicating the up direction of the transform
     */
    Transform(const Vec3& pos, const Vec3& target, const Vec3& up);


    /**
     * \brief Get Forward pointing direction for a transform
     */
    Vec3 GetForward();

    /**
     * \brief Set position of the transform while retaining rotation
     * \param pos 
     */
    void SetPosition(const Vec3& pos);

    /**
     * \brief Set position with a 2d vector 
     * \param pos
     */
    void SetPosition2D(const Vec2& pos);

    /**
     * \brief Update Rotation around the X-Axis
     * \param row 
     */
    void UpdateRow(float row);

    /**
     * \brief Update rotation around the Y-Axis
     * \param pitch 
     */
    void UpdatePitch(float pitch);

    /**
     * \brief Update rotation around the Z-Axis
     * \param yaw 
     */
    void UpdateYaw(float yaw);

    /**
     * \brief Update position by delta and rotation by rot
     * \param delta 
     * \param rot 
     */
    void Update(const Vec3& delta, const Quat& rot);

    /**
     * \brief Add current position and rotation to a point
     * \param point 
     * \return The Transform Vec3
     */
    Vec3 TransformVec3(const Vec3& point) const;

    /**
     * \brief Transform a normal vector by rotation it to the current
     *        rotation
     * NOTE: The current implementation does not support non-uniform scaling
     *       the return normal will NOT be correct if there is scaling applied to the
     *       transform (FIXME)
     * \param normal 
     * \return 
     */
    Vec3 TransformNormal(const Vec3& normal) const;
};
