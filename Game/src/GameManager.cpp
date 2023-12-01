#include "stdafx.h"
#include "GameManager.h"
#include "Utils.h"
#include "Camera.h"
#include "../App/AppSettings.h"

extern Coordinator gCoordinator;
constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

void GameManager::Setup()
{
	Entity camEntity = gCoordinator.CreateEntity();
	Camera cam = Camera(Vec3(0, -10, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), 90.0f, ASPECT_RATIO, 0.1, 1000);
	gCoordinator.AddComponent<Camera>(camEntity, cam);
	/* 
	 setup tests meshes
	*/
	Entity e = gCoordinator.CreateEntity();
	Mesh mesh_obj;
	Utils::LoadFromObjectFile("./Assets/teapot.obj", mesh_obj);
	Transform transform = Transform(Vec3(0, 0, 0), Quat(Vec3(1, 0, 0), 3.141f));
	gCoordinator.AddComponent<Mesh>(e, mesh_obj);
	gCoordinator.AddComponent<Transform>(e, transform);

}

void GameManager::Render()
{
	renderer->Render();
}

void GameManager::Update(float deltaTime)
{
	debugCam->Move(deltaTime);
}
