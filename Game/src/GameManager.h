#pragma once
#include "Coordinator.h"
#include "Renderer.h"
#include "DebugCamera.h"

class GameManager {
public:
	GameManager() {}

	void Setup();

	void Render();

	void Update(float deltaTime);

private:
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<DebugCamera> debugCam;

};