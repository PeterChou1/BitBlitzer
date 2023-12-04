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
	Camera cam = Camera(Vec3(0, 0, 0), Vec3(0, 0, 15), Vec3(0, 1, 0), 90.0f, ASPECT_RATIO, 0.1, 1000);
	gCoordinator.AddComponent<Camera>(camEntity, cam);
	/* 
	 setup tests meshes
	*/
	Entity e = gCoordinator.CreateEntity();
	Mesh mesh_obj;

    mesh_obj.tris = {
        // SOUTH face
        Triangle(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(1, 1, 0), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0)),
        Triangle(Vec3(0, 0, 0), Vec3(1, 1, 0), Vec3(1, 0, 0), Vec2(0, 1), Vec2(1, 0), Vec2(1, 1)),
    
        // EAST face
        Triangle(Vec3(1, 0, 0), Vec3(1, 1, 0), Vec3(1, 1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0)),
        Triangle(Vec3(1, 0, 0), Vec3(1, 1, 1), Vec3(1, 0, 1), Vec2(0, 1), Vec2(1, 0), Vec2(1, 1)),
    
        // NORTH face
        Triangle(Vec3(1, 0, 1), Vec3(1, 1, 1), Vec3(0, 1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0)),
        Triangle(Vec3(1, 0, 1), Vec3(0, 1, 1), Vec3(0, 0, 1), Vec2(0, 1), Vec2(1, 0), Vec2(1, 1)),
    
        // WEST face
        Triangle(Vec3(0, 0, 1), Vec3(0, 1, 1), Vec3(0, 1, 0), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0)),
        Triangle(Vec3(0, 0, 1), Vec3(0, 1, 0), Vec3(0, 0, 0), Vec2(0, 1), Vec2(1, 0), Vec2(1, 1)),
    
        // TOP face
        Triangle(Vec3(0, 1, 0), Vec3(0, 1, 1), Vec3(1, 1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0)),
        Triangle(Vec3(0, 1, 0), Vec3(1, 1, 1), Vec3(1, 1, 0), Vec2(0, 1), Vec2(1, 0), Vec2(1, 1)),
    
        // BOTTOM face
        Triangle(Vec3(1, 0, 1), Vec3(0, 0, 1), Vec3(0, 0, 0), Vec2(1, 1), Vec2(0, 1), Vec2(0, 0)),
        Triangle(Vec3(1, 0, 1), Vec3(0, 0, 0), Vec3(1, 0, 0), Vec2(1, 1), Vec2(0, 0), Vec2(1, 0)),
    };
	Utils::LoadFromObjectFile("./Assets/teapot.obj", mesh_obj);
	Transform transform = Transform(Vec3(0, 0, 3), Quat(Vec3(1, 0, 0), - 3.141 / 2));
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
