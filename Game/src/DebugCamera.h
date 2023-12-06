#pragma once
#include "System.h"
#include "Camera.h"
#include <memory>

class DebugCamera : System {
public:
	void Move(float deltaTime);
	void Render();
private:
	Camera* cam;
};