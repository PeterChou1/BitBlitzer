#include "stdafx.h"
#include "Camera.h"
#include <iostream>



Camera::Camera(Vec3& pos, Vec3& target, Vec3& up, 
	           float fov, float aspect_ratio, float nearplane, float farplane) : pos(pos), target(target), up(up), nearplane(nearplane), farplane(farplane)
{
	forward = (pos - target).Normalize();
	cam_to_world = Transform(pos, target, up);
	world_to_cam = cam_to_world.Inverse();
	proj.PerspectiveOpenGL(fov, aspect_ratio, nearplane, farplane);
}

void Camera::UpdatePos(const Vec3& delta, const Quat& rot)
{

	forward = rot.RotatePoint(forward);
	pos += delta;
	target = pos + forward;
	cam_to_world = Transform(pos, target, up);
	world_to_cam = cam_to_world.Inverse();
}
