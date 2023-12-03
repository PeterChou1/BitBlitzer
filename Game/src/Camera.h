#pragma once
#include "Mesh.h"
#include "Mat4.h"


class Camera {
public:
	Mat4 proj;
	Transform cam_to_world;
	Transform world_to_cam;
	Vec3 pos;
	Vec3 target;
	Vec3 forward;
	Vec3 up;
	float nearplane;
	float farplane;



	Camera() {};
	Camera(Vec3& pos, 
		   Vec3& target, 
		   Vec3& up, 
		   float fov, float aspect_ratio, float nearplane, float farplane);

	void UpdatePos(const Vec3& pos, const Quat& rot);
};