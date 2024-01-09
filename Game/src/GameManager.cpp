#include "stdafx.h"
#include "GameManager.h"
#include "ECSManager.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "DepthBuffer.h"
#include "PixelBuffer.h"
#include "ClippedTriangleBuffer.h"
#include "RenderConstants.h"
#include "Scene.h"
#include "Tiles.h"
#include "../App/AppSettings.h"

extern ECSManager ECS;


Scene scene;


void GameManager::Setup()
{
    // setup opengl rendering instance
    App::SetupGL();

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

    // Initialize Render Systems
    m_VertexShader = std::make_unique<VertexShader>(VertexShader());
    m_Clipper = std::make_unique<Clipper>(Clipper());
    m_Rasterizer = std::make_unique<Rasterizer>(Rasterizer());
    m_FragmentShader = std::make_unique<FragmentShader>(FragmentShader());
    m_DebugCamera = std::make_unique<DebugCamera>(DebugCamera());
    m_DebugMesh = std::make_unique<DebugMesh>(DebugMesh());
    m_MeshHandler = std::make_unique<MeshHandler>(MeshHandler());

    scene.Setup();
}

void GameManager::Update(float deltaTime)
{
    m_DebugCamera->Move(deltaTime);
    m_DebugMesh->Update(deltaTime);
    m_MeshHandler->Update();
}

void GameManager::Render()
{
    // Render Pipeline
    m_VertexShader->Shade();
    m_Clipper->Clip();
    m_Rasterizer->Rasterize();
    m_FragmentShader->Shade();
    m_DebugCamera->Render();
    // Clear Render Pipeline to get ready for next pass
    ECS.GetResource<ClippedTriangleBuffer>()->ResetResource();
    ECS.GetResource<Tiles>()->ResetResource();
    ECS.GetResource<DepthBuffer>()->ResetResource();
    ECS.GetResource<PixelBuffer>()->ResetResource();
    ECS.GetResource<ColorBuffer>()->ResetResource();
}
