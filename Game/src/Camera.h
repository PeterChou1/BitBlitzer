#pragma once
#include "Mesh.h"
#include "Mat4.h"


class Camera {
public:
	Mat4 proj;
	Transform transform;
	Transform world_to_cam;
	Vec3 pos;
	Vec3 target;
	Vec3 forward;
	Vec3 up;
	float nearplane;
	float farplane;
	float screenHeight;
	float screenWidth;


	Camera() {};

	Camera(Vec3& pos, 
		   Vec3& target, 
		   Vec3& up, 
		   float fov, float aspect_ratio, 
		   float nearplane, float farplane);

	void UpdatePos(const Vec3& pos, const Quat& rot);

	void ToRasterSpace(Vec4& point);

	Mat4 CameraToWorldMatrix();

	Mat4 WorldToCameraMatrix();

private:


};