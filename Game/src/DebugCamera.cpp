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
		Vec3& camPos = cam.pos;
		std::cout << "position: " << cam.pos.toString() << std::endl;

		std::cout << "target: " << (cam.target - cam.pos).toString() << std::endl;

		float speed = 10 * deltaTime / 1000;
		
		if (App::GetController().GetLeftThumbStickX() > 0.5f)
		{
			camPos.x -= speed;
		}
		if (App::GetController().GetLeftThumbStickX() < -0.5f)
		{
			camPos.x += speed;
		}


		if (App::GetController().GetLeftThumbStickY() > 0.5f)
		{
			camPos.z -= speed;
		}
		if (App::GetController().GetLeftThumbStickY() < -0.5f)
		{
			camPos.z += speed;
		}

		cam.UpdatePos(camPos);
	}
}
