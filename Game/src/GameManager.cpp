#include "stdafx.h"
#include "../App/AppSettings.h"
#include "GameManager.h"
#include "Utils.h"
#include "Camera.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "GraphicsBuffer.h"
#include "../App/app.h"
#include <iostream>

extern Coordinator gCoordinator;
constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

void Scene::Setup()
{
    // Setup Camera
	Entity camEntity = gCoordinator.CreateEntity();
	Camera cam = Camera(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0), APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, 90.0f, ASPECT_RATIO, 0.1, 100);
	

    // Setup Rendering
    Entity dBufferEntity = gCoordinator.CreateEntity();
    DepthBuffer depthBuffer = DepthBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);


    Entity graphicsEntity = gCoordinator.CreateEntity();
    GraphicsBuffer g = GraphicsBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);
    

    Entity testRenderEntity = gCoordinator.CreateEntity();
    Transform t1 = Transform(Vec3(0, 0, 10), Quat(Vec3(1, 0, 0), 0.0));// 3.14 / 2));

    MeshInstance meshI;
    // Setup 
    Utils::LoadInstance("./Assets/triangle.obj", meshI, true , false, testRenderEntity);

    std::cout << "Triangle Count: " << meshI.indices.size() / 3 << std::endl;
    g.AddMeshInstance(meshI);

    Entity e = gCoordinator.CreateEntity();
    Mesh mesh_obj = Mesh(meshI);
    Transform t2 = Transform(Vec3(0, 0, 10), Quat(Vec3(1, 0, 0), 0.0));// 3.14 / 2));
    SimpleTexture texture = SimpleTexture("./Assets/spot_texture.png");

    gCoordinator.AddComponent<Mesh>(e, mesh_obj);
    gCoordinator.AddComponent<Transform>(e, t2);
    gCoordinator.AddComponent<SimpleTexture>(e, texture);
    gCoordinator.AddComponent<Camera>(camEntity, cam);
    gCoordinator.AddComponent<DepthBuffer>(dBufferEntity, depthBuffer);
    gCoordinator.AddComponent<GraphicsBuffer>(graphicsEntity, g);
    gCoordinator.AddComponent<Transform>(testRenderEntity, t1);


    debugCam = std::make_shared<DebugCamera>(DebugCamera(gCoordinator.GetComponent<Camera>(camEntity)));
    rendererM = std::make_shared<RendererM>(RendererM(gCoordinator.GetComponent<GraphicsBuffer>(graphicsEntity), gCoordinator.GetComponent<Camera>(camEntity)));
    //renderer = std::make_shared<Renderer>(Renderer(cam, depthBuffer));

}

void Scene::Render()
{
    rendererM->Render();
    // renderer->Render();
    debugCam->Render();
}

void Scene::Update(float deltaTime)
{
	debugCam->Move(deltaTime);
}
