#include "stdafx.h"
#include "GameManager.h"
#include "Utils.h"

void GameManager::Setup()
{
	coordinator.Init();
	renderer = coordinator.RegisterSystem<Renderer>();
	/* 
	 setup tests meshes
	*/
	Entity e = coordinator.CreateEntity();
	Mesh mesh_obj;
	Utils::LoadFromObjectFile("./Assets/teapot.obj", mesh_obj);
	Transform transform = Transform(Vec3(0, 3, 8), Quat(Vec3(1, 0, 0), 3.141f));
	coordinator.AddComponent<Mesh>(e, mesh_obj);
	coordinator.AddComponent<Transform>(e, transform);

}

void GameManager::Render()
{

}

void GameManager::Update(float deltaTime)
{

}
