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
    bool addMesh,
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

    if (addMesh)
        ECS.AddComponent<Mesh>(entity, Mesh(WoodCube));
}

void CreateLevelGeometry(Vec3 location, Quat rotation, float scale, ObjAsset asset)
{
    Entity meshEntity = ECS.CreateEntity();
    Transform transform = Transform(location, rotation);
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
    CreateRigidBodyRect(x, y, 1.0f, 6.0, 0.0f, false, true, true, Obstacle);
    // Right Leg
    CreateRigidBodyRect(x + 5.0f, y + 0.0f, 1.0f, 6.0, 0.0, false, true, true, Obstacle);
    // Top Cover
    CreateRigidBodyRect(x + 2.5f, y + 3.5, 10.0f, 1.0f, 0.0, false, true, true, Obstacle);
    // Player Owned Cow
    CreateCows(x + 2.5f, y + 6.0, owner);
}


void SetupPhysicsBodies()
{
    CreateRigidBodyRect(0.0f, -6.5f, 500.0f, 2.0f, 0.0f, true, true, false, Obstacle);
    SetupStructure(-60.0, -2.5, Player1);
    SetupStructure(60.0, -2.5, Player2);
    SetupStructure(60.0, 3.5, Player2);

    // Kill box destroys anything out of bounds
    CreateRigidBodyRect(-150.0f, 0.0f, 100.0f, 1000.0f, 0.0f, true, false, false, KillBox);
    CreateRigidBodyRect(150.0f, 0.0f, 100.0f, 1000.0f, 0.0f, true, false, false, KillBox);
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
    m_Lights = ECS.GetResource<Lighting>();
}

void Level1::Setup()
{
    // Load level asset
    ECS.GetResource<CubeMap>()->LoadCubeMap(SkyBox);
    auto& server = AssetServer::GetInstance();
    server.LoadLevelAssets({ 
        SpotFlipped, Spot,
        SlingShot, Dirt,
        WoodCube, Grass,
        Tree
    });

    // Set Scene Light
    m_Lights->SetLight(Vec3(0, 50, -20), Vec3(1, 1, 1), 40.0);

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
    // Set Positions Camera can lerp to
    m_CamController->Player1ViewArea = Vec3(-30.0, 10, 35);
    m_CamController->Player2ViewArea = Vec3(30.0, 10, 35);
    // Set the follow distance when following a projectile
    m_CamController->FollowProjectileZoom = 20.0f;

    // Set up projectile launch controller
    m_LaunchController->Player1LaunchPoint = Vec3(-40.0, 2.0f, 0.0f);
    m_LaunchController->Player2LaunchPoint = Vec3(40.0, 2.0f, 0.0f);


    // Register collider callbacks
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileCowCollider>());
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileKillBoxCollider>());
    m_ColliderCallback->RegisterCallback(std::make_shared<ProjectileObstaclesCollider>());

    // Add level geometry
    DecorateScene();
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

void Level1::DecorateScene()
{
    // Add level geometry

    // Sling Shot
    Vec3 Player1SlingShot = m_LaunchController->Player1LaunchPoint - Vec3(0.0, 7.0, 0.0);
    Quat rotation1 = Quat(Vec3(0, 1, 0), -40.0f * 3.141 / 180.0f);
    CreateLevelGeometry(Player1SlingShot, rotation1, 20.0, SlingShot);

    Vec3 Player2SlingShot = m_LaunchController->Player2LaunchPoint - Vec3(0.0, 7.0, 0.0);
    Quat rotation2 = Quat(Vec3(0, 1, 0), 40.0f * 3.141 / 180.0f);
    CreateLevelGeometry(Player2SlingShot, rotation2, 20.0, SlingShot);

    // Environment
    CreateLevelGeometry(Vec3(0, -205.5, 0), Quat(), 200.0, Dirt);
    CreateLevelGeometry(Vec3(0, -5.0, 0), Quat(), 200, Grass);

    // Create some trees
    for (int i = -150; i < 150; i += 20)
    {
        CreateLevelGeometry(Vec3(i + 0, -5.0, -10.0), Quat(), 2.0, Tree);
        CreateLevelGeometry(Vec3(i + 2, -5.0, -10.0), Quat(), 1.0, Tree);
        CreateLevelGeometry(Vec3(i + -5, -5.0, -10.0), Quat(), 1.0, Tree);
        CreateLevelGeometry(Vec3(i + 8, -5.0, -10.0), Quat(), 3.0, Tree);
        CreateLevelGeometry(Vec3(i + 6, -5.0, -10.0), Quat(), 1.5, Tree);
        CreateLevelGeometry(Vec3(i + -5, -5.0, -10.0), Quat(), 1.2, Tree);
    }
}
