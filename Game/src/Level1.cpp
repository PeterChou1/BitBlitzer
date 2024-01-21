#include "stdafx.h"

#include "Level1.h"
#include "Assets.h"
#include "AssetServer.h"
#include "Cows.h"
#include "CubeMap.h"
#include "ECSManager.h"
#include "Entity.h"
#include "GameState.h"
#include "Mesh.h"
#include "ProjectileCowCollider.h"
#include "ProjectileKillBoxCollider.h"
#include "ProjectileObstaclesCollider.h"
#include "RigidBody.h"
#include "Transform.h"


extern ECSManager ECS;

void CreateRigidBodyRect(
    float x, float y,
    float width, float height,
    float rotation, bool isStatic,
    bool collidable,
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
    rigidbody.Collidable = collidable;
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

void CreateCows(float x, float y, Owner owner)
{
    Entity entity = ECS.CreateEntity();
    auto transform = Transform(Vec3(x, y, 0.0f), Quat(Vec3(0.0f, 1.0f, 0.0f), 3.141));
    transform.Scale(1.5);
    ECS.AddComponent<Transform>(entity, transform);
    auto rigidbody = RigidBody(2.0f, 2.0f);
    rigidbody.Color = Vec3(1.0, 0.0, 0.0);
    rigidbody.Category = Cow;
    ECS.AddComponent<RigidBody>(entity, rigidbody);
    ECS.AddComponent<PlayerOwnedCow>(entity, PlayerOwnedCow(owner));
    ECS.AddComponent<Mesh>(entity, Mesh(Spot));
}

void SetupStructure(float x, float y, Owner owner)
{
    // Left Leg
    CreateRigidBodyRect(x, y, 1.0f, 6.0, 0.0f, false, true, Obstacle);
    // Right Leg
    CreateRigidBodyRect(x + 5.0f, y + 0.0f, 1.0f, 6.0, 0.0, false, true, Obstacle);
    // Top Cover
    CreateRigidBodyRect(x + 2.5f, y + 3.5, 10.0f, 1.0f, 0.0, false, true, Obstacle);
    // Player Owned Cow
    CreateCows(x + 2.5f, y + 6.0, owner);
}


void SetupPhysicsBodies()
{
    CreateRigidBodyRect(0.0f, -6.5f, 500.0f, 2.0f, 0.0f, true, true, Obstacle);
    SetupStructure(-60.0, -2.5, Player1);
    SetupStructure(60.0, -2.5, Player2);
    SetupStructure(60.0, 3.5, Player2);

    // Kill box destroys anything out of bounds
    CreateRigidBodyRect(-150.0f, 0.0f, 100.0f, 1000.0f, 0.0f, true, false, KillBox);
    CreateRigidBodyRect(150.0f, 0.0f, 100.0f, 1000.0f, 0.0f, true, false, KillBox);
}

void Level1::Start()
{
    // Registers System for level 1
    m_CamController = std::make_unique<CameraController>();
    m_LaunchController = std::make_unique<LaunchController>();
    m_UISystem = std::make_unique<UISystem>();
    m_WinConditionChecker = std::make_unique<WinConditionChecker>();
    m_ColliderCallback = ECS.GetResource<ColliderCallbackSystem>();
    m_GameState = ECS.GetResource<GameState>();
}

void Level1::Setup()
{
    // Load level asset
    // ECS.GetResource<CubeMap>()->LoadCubeMap(SkyBox);
    auto& server = AssetServer::GetInstance();
    server.LoadLevelAssets({ SpotFlipped, Spot, SlingShot});

    // Set Up Systems
    m_CamController->Setup(Vec3(0.0f, 0.0f, 50.0f));
    m_LaunchController->Setup();
    m_UISystem->Setup();
    m_WinConditionChecker->Setup();

    // Set Camera Bounds
    m_CamController->Left = -100.0f;
    m_CamController->Right = 100.0f;
    m_CamController->Bottom = -5.0f;
    m_CamController->Top = 10.0f;
    m_CamController->ZoomIn = 15.0f;
    m_CamController->ZoomOut = 50.0f;
    m_CamController->Player1ViewArea = Vec3(-30.0, 10, 35);
    m_CamController->Player2ViewArea = Vec3(30.0, 10, 35);
    m_CamController->FollowProjectileZoom = 20.0f;

    // Set up projectile launch controller
    m_LaunchController->Player1LaunchPoint = Vec3(-40.0, 2.0f, 0.0f);
    m_LaunchController->Player2LaunchPoint = Vec3(40.0, 2.0f, 0.0f);

    // Add level geometry
    Vec3 Player1SlingShot = m_LaunchController->Player1LaunchPoint - Vec3(0.0, 7.0, 0.0);
    CreateLevelGeometry(Player1SlingShot, 20.0, -40.0f, SlingShot);
    Vec3 Player2SlingShot = m_LaunchController->Player2LaunchPoint - Vec3(0.0, 7.0, 0.0);
    CreateLevelGeometry(Player2SlingShot, 20.0, 40.0f, SlingShot);

    // Register collider callbacks
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileCowCollider>());
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileKillBoxCollider>());
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileObstaclesCollider>());

    // Register physics collider
    SetupPhysicsBodies();
}

void Level1::Update(float deltaTime)
{
    m_CamController->Update(deltaTime);
    m_LaunchController->Update();
    m_WinConditionChecker->Update();
}

void Level1::Render()
{
    m_LaunchController->Render();
    m_UISystem->Render();
}
