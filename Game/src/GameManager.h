#pragma once
#include "Coordinator.h"
#include "Renderer.h"

class GameManager {
public:
	GameManager() {}

	void Setup();

	void Render();

	void Update(float deltaTime);

private:
	Coordinator coordinator;
	std::shared_ptr<Renderer> renderer;

};