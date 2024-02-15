#include "stdafx.h"

#include "GameManager.h"
#include "ECSManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "DepthBuffer.h"
#include "PixelBuffer.h"
#include "ClippedTriangleBuffer.h"
#include "GameState.h"
#include "Lighting.h"
#include "RenderConstants.h"
#include "SpriteManager.h"
#include "Tiles.h"
#include "../App/AppSettings.h"

// Initialized at the start of the Game
extern ECSManager ECS;
// User defined function to Register Scenes

void GameManager::Setup()
{
    // Register Resources Required For Rendering
    ECS.RegisterResource(Camera());
    ECS.RegisterResource(VertexBuffer());
    ECS.RegisterResource(IndexBuffer());
    ECS.RegisterResource(ClippedTriangleBuffer());
    ECS.RegisterResource(Tiles(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT));
    ECS.RegisterResource(DepthBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT));
    ECS.RegisterResource(PixelBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT));
    ECS.RegisterResource(ColorBuffer(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT));
    ECS.RegisterResource(RenderConstants());
    ECS.RegisterResource(CubeMap());
    ECS.RegisterResource(GameState());
    ECS.RegisterResource(ColliderCallbackSystem());
    ECS.RegisterResource(Lighting());
    ECS.RegisterResource(SpriteManager());

    // Initialize Common Systems
    m_VertexShader = std::make_unique<VertexShader>();
    m_Clipper = std::make_unique<Clipper>();
    m_Rasterizer = std::make_unique<Rasterizer>();
    m_FragmentShader = std::make_unique<FragmentShader>();
    m_MeshHandler = std::make_unique<MeshHandler>();
    m_PhysicsSystem = std::make_unique<PhysicsSystem>();
    m_TimerSystem = std::make_unique<TimerSystem>();

    // Debug Systems 
    m_DebugCamera = std::make_unique<DebugCamera>();
    m_DebugMesh = std::make_unique<DebugMesh>();
    m_DebugPhysicsRender = std::make_unique<DebugPhysicsRenderer>();
}

void GameManager::Update(float deltaTime)
{
    assert(m_SceneMap.count(m_ActiveScene) > 0 && "Active Scene Name Not registered");
    ECS.GetResource<SpriteManager>()->Update(deltaTime);
    // m_DebugCamera->Update(deltaTime);
    // m_DebugMesh->Update(deltaTime);
    m_TimerSystem->Update(deltaTime);
    m_PhysicsSystem->Update(deltaTime);
    // m_DebugPhysicsRender->Update(deltaTime);
    m_SceneMap[m_ActiveScene]->Update(deltaTime);
    m_MeshHandler->Update();
}

void GameManager::Render()
{
    assert(m_SceneMap.count(m_ActiveScene) > 0 && "Active Scene Name Not registered");
    // Render Pipeline
    m_VertexShader->Shade();
    m_Clipper->Clip();
    m_Rasterizer->Rasterize();
    m_FragmentShader->Shade();
    // Debug Stuff
    // m_DebugCamera->Render();
    // m_DebugPhysicsRender->Render();
    // Rendering for the active scene
    m_SceneMap[m_ActiveScene]->Render();
    // Draw 2d sprite effects
    ECS.GetResource<SpriteManager>()->Render();
    // Clear Render Pipeline to get ready for next render pass
    ECS.GetResource<ClippedTriangleBuffer>()->ResetResource();
    ECS.GetResource<Tiles>()->ResetResource();
    ECS.GetResource<DepthBuffer>()->ResetResource();
    ECS.GetResource<PixelBuffer>()->ResetResource();
    ECS.GetResource<ColorBuffer>()->ResetResource();
}

void GameManager::RegisterScene(const std::string& sceneName, std::unique_ptr<Scene> scene)
{
    m_SceneMap[sceneName] = std::move(scene);
    m_SceneMap[sceneName]->Start();
}

void GameManager::SetActiveScene(const std::string& sceneName)
{
    ECS.Reset();
    m_ActiveScene = sceneName;
    assert(m_SceneMap.count(sceneName) > 0 && "Scene name does not exist");
    m_SceneMap[m_ActiveScene]->Setup();
}

