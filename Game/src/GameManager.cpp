#include "stdafx.h"
#include "GameManager.h"
#include "Utils.h"
#include "Camera.h"
#include "SimpleTexture.h"
#include "../App/AppSettings.h"
#include <iostream>

extern Coordinator gCoordinator;
constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

void GameManager::Setup()
{
    // test load texture
	Entity camEntity = gCoordinator.CreateEntity();
	Camera cam = Camera(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, -1, 0), 90.0f, ASPECT_RATIO, 0.1, 1000);
	gCoordinator.AddComponent<Camera>(camEntity, cam);
	/* 
	 setup tests meshes
	*/
	Entity e = gCoordinator.CreateEntity();
	Mesh mesh_obj;

    mesh_obj.tris = {
        // SOUTH face
        Triangle(Vertex(Vec3(0, 0, 0), Vec2(0, 1)), Vertex(Vec3(0, 1, 0), Vec2(0, 0)), Vertex(Vec3(1, 1, 0), Vec2(1, 0))),
        Triangle(Vertex(Vec3(0, 0, 0), Vec2(0, 1)), Vertex(Vec3(1, 1, 0), Vec2(1, 0)), Vertex(Vec3(1, 0, 0), Vec2(1, 1))),
    
        // EAST face
        Triangle(Vertex(Vec3(1, 0, 0), Vec2(0, 1)), Vertex(Vec3(1, 1, 0), Vec2(0, 0)), Vertex(Vec3(1, 1, 1), Vec2(1, 0))),
        Triangle(Vertex(Vec3(1, 0, 0), Vec2(0, 1)), Vertex(Vec3(1, 1, 1), Vec2(1, 0)), Vertex(Vec3(1, 0, 1), Vec2(1, 1))),
    
        // NORTH face
        Triangle(Vertex(Vec3(1, 0, 1), Vec2(0, 1)), Vertex(Vec3(1, 1, 1), Vec2(0, 0)), Vertex(Vec3(0, 1, 1), Vec2(1, 0))),
        Triangle(Vertex(Vec3(1, 0, 1), Vec2(0, 1)), Vertex(Vec3(0, 1, 1), Vec2(1, 0)), Vertex(Vec3(0, 0, 1), Vec2(1, 1))),
    
        // WEST face
        Triangle(Vertex(Vec3(0, 0, 1), Vec2(0, 1)), Vertex(Vec3(0, 1, 1), Vec2(0, 0)), Vertex(Vec3(0, 1, 0), Vec2(1, 0))),
        Triangle(Vertex(Vec3(0, 0, 1), Vec2(0, 1)), Vertex(Vec3(0, 1, 0), Vec2(1, 0)), Vertex(Vec3(0, 0, 0), Vec2(1, 1))),
    
        // TOP face
        Triangle(Vertex(Vec3(0, 1, 0), Vec2(0, 1)), Vertex(Vec3(0, 1, 1), Vec2(0, 0)), Vertex(Vec3(1, 1, 1), Vec2(1, 0))),
        Triangle(Vertex(Vec3(0, 1, 0), Vec2(0, 1)), Vertex(Vec3(1, 1, 1), Vec2(1, 0)), Vertex(Vec3(1, 1, 0), Vec2(1, 1))),
    
        // BOTTOM face
        Triangle(Vertex(Vec3(1, 0, 1), Vec2(0, 1)), Vertex(Vec3(0, 0, 1), Vec2(0, 0)), Vertex(Vec3(0, 0, 0), Vec2(1, 0))),
        Triangle(Vertex(Vec3(1, 0, 1), Vec2(0, 1)), Vertex(Vec3(0, 0, 0), Vec2(1, 0)), Vertex(Vec3(1, 0, 0), Vec2(1, 1))),
    };
	// Utils::LoadFromObjectFile("./Assets/teapot.obj", mesh_obj);
	Transform transform = Transform(Vec3(0, 0, 3), Quat(Vec3(1, 0, 0), 0));
    SimpleTexture texture = SimpleTexture("./Assets/mario.png");

	gCoordinator.AddComponent<Mesh>(e, mesh_obj);
	gCoordinator.AddComponent<Transform>(e, transform);
    gCoordinator.AddComponent<SimpleTexture>(e, texture);

}

void GameManager::Render()
{
	renderer->Render();
    debugCam->Render();
}

void GameManager::Update(float deltaTime)
{
	debugCam->Move(deltaTime);
}
