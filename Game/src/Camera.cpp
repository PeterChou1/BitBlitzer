#include "stdafx.h"
#include "Camera.h"



Camera::Camera(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	cam_to_world = Transform(pos, target, up);
	world_to_cam = cam_to_world.Inverse();
}
