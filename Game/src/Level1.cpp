#include "stdafx.h"

#include "Level1.h"
#include "Assets.h"
#include "AssetServer.h"
#include "ECSManager.h"
#include "Entity.h"
#include "GameState.h"
#include "Mesh.h"
#include "ProjectileCowCollider.h"
#include "RigidBody.h"
#include "Transform.h"


extern ECSManager ECS;

void CreateRigidBodyRect(
    float x, float y,
    float width, float height,
    float rotation, bool isStatic,
    ColliderCategory category
)
{
    float radians = (3.141f / 180.0f) * rotation;
    Entity entity = ECS.CreateEntity();
    auto transform = Transform(Vec3(x, y, 0.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), radians));
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(width, height);
    if (isStatic)
        rigidbody.SetStatic();
    rigidbody.Color = Vec3(1.0, 0.0, 0.0);
    rigidbody.Category = category;
    ECS.AddComponent<RigidBody>(entity, rigidbody);
}

void CreateRigidBodyCircle(
    float x, float y,
    float radius, bool isStatic,
    ColliderCategory category
)
{
    Entity entity = ECS.CreateEntity();
    auto transform = Transform(Vec3(x, y, 0.0f), Quat(Vec3(0.0f, 0.0f, 1.0f), 0.0));
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(radius);
    if (isStatic)
        rigidbody.SetStatic();
    rigidbody.Color = Vec3(1.0, 0.0, 0.0);
    rigidbody.Category = category;
    ECS.AddComponent<RigidBody>(entity, rigidbody);
}

void CreateLevelGeometry(Vec3 location, float scale, float angleDegree, ObjAsset asset)
{
    Entity meshEntity = ECS.CreateEntity();
    float radian = angleDegree * 3.141 / 180.0f;
    Transform transform = Transform(location, Quat(Vec3(0.0, 1.0, 0.0), radian));
    transform.Scale(scale);
    ECS.AddComponent(meshEntity, transform);
    ECS.AddComponent(meshEntity, Mesh(asset));
}


void SetupStructure(float x, float y)
{
    CreateRigidBodyRect(x + 0.0f, y + 0.0f, 1.0f, 6.0, 0.0f, false, Wall);
    CreateRigidBodyRect(x + 5.0f, y + 0.0f, 1.0f, 6.0, 0.0, false, Wall);
    CreateRigidBodyRect(x + 2.5f, y + 3.5, 10.0f, 1.0f, 0.0, false, Wall);
}


void SetupLevel()
{
    CreateRigidBodyRect(0.0f, -6.5f, 100.0f, 2.0f, 0.0f, true, Ground);
    SetupStructure(0.0, -2.5);
    //SetupStructure(10.0, 0.0);
}

void Level1::Start()
{
    // Registers System for level 1
    m_CamController = std::make_unique<CameraController>();
    m_LaunchController = std::make_unique<LaunchController>();
    m_ColliderCallback = ECS.GetResource<ColliderCallbackSystem>();
    m_GameState = ECS.GetResource<GameState>();
}

void Level1::Setup()
{
    auto& server = AssetServer::GetInstance();
    server.LoadLevelAssets({SlingShot, Spot});
    m_CamController->Setup(Vec3(0.0f, 0.0f, 50.0f));
    m_LaunchController->Setup();
    m_GameState->LaunchPoint = Vec3(-40.0, 0.0f, 0.0f);
    // Add level geometry
    CreateLevelGeometry(
        m_GameState->LaunchPoint - Vec3(0.0, 5.0, 0.0),
        20.0, -40.0f, SlingShot
    );
    // Register collider callbacks
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileCowCollider>());
    // Register physics collider
    SetupLevel();
}

void Level1::Update(float deltaTime)
{
    m_CamController->Update(deltaTime);
    m_LaunchController->Update(deltaTime);
}

void Level1::Render()
{
    m_LaunchController->Render();
}
