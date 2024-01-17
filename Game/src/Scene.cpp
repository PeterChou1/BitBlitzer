#include "stdafx.h"

#include "Scene.h"
#include "Camera.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"


extern ECSManager ECS;

void CreateRigidBodyRect(float x, float y, float width, float height, float rotation)
{
    float radians = (3.141f  / 180.0f) * rotation;

    Entity entity = ECS.CreateEntity();
    auto transform = Transform(Vec3(x, y, 5.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), radians));
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(width, height);
    rigidbody.SetStatic(true);
    ECS.AddComponent<RigidBody>(entity, rigidbody);
}

void CreateRigidBodyCircle(float x, float y, float radius)
{
    Entity entity = ECS.CreateEntity();
    auto transform = Transform(Vec3(x, y, 5.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), 0.0));
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(radius);
    rigidbody.SetStatic(true);
    ECS.AddComponent<RigidBody>(entity, rigidbody);
} 

void Scene::Setup()
{
    // Loaded all asset that the current scene needs
    // Loaded all asset that the current scene needs
    std::set<ObjAsset> levelAssets({ Spot });
    AssetServer& loader = AssetServer::GetInstance();
    loader.LoadLevelAssets(levelAssets);

    auto camera = ECS.GetResource<Camera>();
    // Setup camera
    camera->SetPosition(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));

    for (int x = 0; x < 2; x++)
    {
        for (int z = 0; z < 2; z++)
        {
            Entity meshEntity = ECS.CreateEntity();
            auto modelTransform = Transform(Vec3(x * 2, 0, 5 + z * 2), Quat(Vec3(1, 0, 0), 0.0));
            ECS.AddComponent<Transform>(meshEntity, modelTransform);
            ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
        }
    }
    // loader.LoadLevelAssets(levelAssets);
    // // Ground
    // CreateRigidBodyRect(0.0f, -5.0f, 30.0, 1.0, 0.0f);
    // // Side walls
    // CreateRigidBodyRect(-15.0f, 0.0f, 1.0, 10.0, 0.0f);
    // CreateRigidBodyRect(15.0f, 0.0f, 1.0, 10.0, 0.0f);

    // Entity meshEntity = ECS.CreateEntity();
    // auto modelTransform = Transform(Vec3(0.0, 0.0, 5.0), Quat(Vec3(0, 0, 1), 0.0));
    // auto rigidbody = RigidBody(1.0f);
    // rigidbody.Controlled = true;
    // ECS.AddComponent<Transform>(meshEntity, modelTransform);
    // ECS.AddComponent<RigidBody>(meshEntity, rigidbody);

    
}


void Scene::Update(float deltaTime)
{

}
