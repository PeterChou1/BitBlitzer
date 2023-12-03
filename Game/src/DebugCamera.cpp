#include "stdafx.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "../App/app.h"
#include <iostream>

extern Coordinator gCoordinator;

void DebugCamera::Move(float deltaTime)
{


	for (Entity const& e : Visit<Camera>(gCoordinator)) {
		
		Camera& cam = gCoordinator.GetComponent<Camera>(e);
		Vec3& delta = Vec3(0, 0, 0);
		Vec3& target = cam.forward;
		Quat r = Quat(cam.up, 0);

		float speed = 10 * (deltaTime / 1000);
		float rotation = 3.141 / 2 * (deltaTime / 1000);
		// 
		if (App::GetController().GetLeftThumbStickX() > 0.5f)
		{
			r *= Quat(cam.up, -rotation);
		}
		if (App::GetController().GetLeftThumbStickX() < -0.5f)
		{
			r *= Quat(cam.up, rotation);
		}

		if (App::GetController().GetLeftThumbStickY() > 0.5f)
		{
			delta += cam.forward * speed;
		}
		if (App::GetController().GetLeftThumbStickY() < -0.5f)
		{
			delta -= cam.forward * speed;
		}

		if (App::IsKeyPressed('Q')) {
			delta += cam.up * speed;
		}

		if (App::IsKeyPressed('E')) {
			delta -= cam.up * speed;
		}

		
		cam.UpdatePos(delta, r);
	}
}
