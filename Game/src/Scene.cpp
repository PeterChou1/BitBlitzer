#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "Shape.h"
#include "RigidBody.h"
extern ECSManager ECS;

void Scene::Setup()
{
    //
    auto camera = ECS.GetResource<Camera>();
    camera->SetPosition(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));

    // Loaded all asset that the current scene needs
    std::set<ObjAsset> levelAssets({Furina, Pacman, Spot});
    AssetServer& loader = AssetServer::GetInstance();
    loader.LoadLevelAssets(levelAssets);

    Entity groundEntity = ECS.CreateEntity();
    auto modelTransform = Transform(Vec3(0.0f, -3.0f, 0.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), 1.0));
    ECS.AddComponent<Transform>(groundEntity, modelTransform);
    auto rb = RigidBody(10.0f, 1.0f);
    rb.SetStatic(true);

    ECS.AddComponent<RigidBody>(groundEntity, rb);
    
    // Setup Rigid Bodies
    for (int x = 0; x < 1; x++)
    {
        Entity meshEntity = ECS.CreateEntity();
        auto modelTransform = Transform(Vec3(x * 3.0f, 3.0, 5.0), Quat(Vec3(1, 0, 0), 0.0));
        // auto meshRB = RigidBody(2.0f, 2.0f);
        ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
        ECS.AddComponent<Transform>(meshEntity, modelTransform);
        //ECS.AddComponent<RigidBody>(meshEntity, meshRB);
    }
    
}


void Scene::Update(float deltaTime)
{

}
