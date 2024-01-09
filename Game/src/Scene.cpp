#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "Mesh.h"

extern ECSManager ECS;

void Scene::Setup()
{
    //
    auto camera = ECS.GetResource<Camera>();
    camera->SetPosition(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0));

    // Loaded all asset that the current scene needs
    std::set<ObjAsset> levelAssets({Furina, Pacman, Spot});
    AssetServer& loader = AssetServer::GetInstance();
    loader.LoadLevelAssets(levelAssets);
    
    // Setup Camera
    for (int x = 0; x < 1; x++)
    {
        for (int z = 0; z < 1; z++)
        {
            Entity meshEntity = ECS.CreateEntity();
            auto modelTransform = Transform(Vec3(x * 2, 0, 5 + z * 2), Quat(Vec3(1, 0, 0), 0.0));
            ECS.AddComponent<Transform>(meshEntity, modelTransform);
            ECS.AddComponent<Mesh>(meshEntity, Mesh(Furina));
        }
    }

}


void Scene::Update(float deltaTime) {}
