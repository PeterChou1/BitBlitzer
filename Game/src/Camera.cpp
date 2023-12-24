#include "stdafx.h"
#include "Camera.h"
#include <iostream>
#include "../App/AppSettings.h"
#include <cassert>



Camera::Camera(Vec3& pos, Vec3& target, Vec3& up, 
	           float fov, float aspect_ratio, float nearplane, float farplane) : pos(pos), target(target), up(up), nearplane(nearplane), farplane(farplane)
{
	forward = (target - pos).Normalize() * -1;
	transform = Transform(pos, target, up);
	world_to_cam = transform.Inverse();
	proj.PerspectiveOpenGL(fov, aspect_ratio, nearplane, farplane);
}

void Camera::UpdatePos(const Vec3& delta, const Quat& rot)
{

	forward = rot.RotatePoint(forward);
	pos += delta;
	target = pos - forward;
	transform = Transform(pos, target, up);
	world_to_cam = transform.Inverse();
}

void Camera::ToRasterSpace(Vec4& point)
{
	point.x = static_cast<int>((point.x + 1) * 0.5 * APP_VIRTUAL_WIDTH);
	point.y = static_cast<int>((point.y + 1) * 0.5 * APP_VIRTUAL_HEIGHT);
	assert(0 <= point.x && point.x <= APP_VIRTUAL_WIDTH);
	assert(0 <= point.y && point.y <= APP_VIRTUAL_HEIGHT);
}

Mat4 Camera::CameraToWorldMatrix()
{
	return transform.affine;
}

Mat4 Camera::WorldToCameraMatrix()
{
	return transform.inverse;
}
