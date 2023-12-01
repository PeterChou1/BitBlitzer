#include "stdafx.h"
#include "Camera.h"
#include <iostream>



Camera::Camera(Vec3& position, Vec3& camTarget, Vec3& camUp, 
	           float fov, float aspect_ratio, float near, float far)
{

	pos = position;
	target = camTarget;
	up = camUp;
	cam_to_world = Transform(position, camTarget, camUp);
	world_to_cam = cam_to_world.Inverse();
	proj.PerspectiveOpenGL(fov, aspect_ratio, near, far);
}

void Camera::UpdatePos(const Vec3& newpos)
{
	cam_to_world = Transform(newpos, target, up);
	world_to_cam = cam_to_world.Inverse();

}
