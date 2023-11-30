#pragma once
#include "Mesh.h"


class Camera {
	Transform cam_to_world;
	Transform world_to_cam;
	Camera(const Vec3& pos, const Vec3& target, const Vec3& up);
};