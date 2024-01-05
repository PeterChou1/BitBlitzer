#include "stdafx.h"
#include "../App/AppSettings.h"
#include "Scene.h"
#include "Camera.h"
#include "AssetServer.h"

extern Coordinator ECS;

constexpr double ASPECT_RATIO = APP_VIRTUAL_WIDTH / APP_VIRTUAL_HEIGHT;

void Scene::Setup()
{
    // Loaded all asset that the current scene needs
    std::set<ObjAsset> levelAssets({Furina});
    AssetServer& loader = AssetServer::GetInstance();
    loader.LoadLevelAssets(levelAssets);

    // Setup Camera
    Entity camEntity = ECS.CreateEntity();
    auto cam = Camera(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0),
                      APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, 90.0f,
                      ASPECT_RATIO, 0.1f, 100);

    for (int x = 0; x < 10; x++)
    {
        for (int z = 0; z < 10; z++)
        {
            Entity meshEntity = ECS.CreateEntity();
            auto modelTransform = Transform(Vec3(x * 2, 0, 5 + z * 2), Quat(Vec3(1, 0, 0), 0.0));
            ECS.AddComponent<Transform>(meshEntity, modelTransform);
            ECS.AddComponent<Mesh>(meshEntity, Mesh(Furina));
        }
    }

    ECS.AddComponent<Camera>(camEntity, cam);
    debugCam = std::make_shared<DebugCamera>(
        DebugCamera(ECS.GetComponent<Camera>(camEntity))
    );
    rendererM = std::make_shared<RendererAVX>(
        RendererAVX(
            APP_VIRTUAL_WIDTH, 
            APP_VIRTUAL_HEIGHT, 
            ECS.GetComponent<Camera>(camEntity)
        )
    );
    debugMesh = std::make_shared<DebugMesh>(
        DebugMesh()
    );
}

void Scene::Render()
{
    rendererM->Render();
    debugCam->Render();
}

void Scene::Update(float deltaTime)
{
    debugCam->Move(deltaTime);
    debugMesh->Update(deltaTime);
    rendererM->Update();
}
