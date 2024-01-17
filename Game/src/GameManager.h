//---------------------------------------------------------------------------------
// GameManager.h
//---------------------------------------------------------------------------------
//
// Manages All Scenes in the Game as well as Systems/Resources
// Common to all scenes such as the Render and Physics System
// 
#pragma once
#include "stdafx.h"

#include <memory>

#include "Clipper.h"
#include "DebugCamera.h"
#include "DebugMesh.h"
#include "DebugPhysicsRenderer.h"
#include "FragmentShader.h"
#include "MeshHandler.h"
#include "PhysicsSystem.h"
#include "Rasterizer.h"
#include "VertexShader.h"

class GameManager
{
public:

    void Setup();

    void Update(float deltaTime);

    void Render();

private:

    std::unique_ptr<VertexShader> m_VertexShader;
    std::unique_ptr<Clipper> m_Clipper;
    std::unique_ptr<Rasterizer> m_Rasterizer;
    std::unique_ptr<FragmentShader> m_FragmentShader;
    std::unique_ptr<MeshHandler> m_MeshHandler;
    std::unique_ptr<DebugCamera> m_DebugCamera;
    std::unique_ptr<DebugMesh> m_DebugMesh;
    std::unique_ptr<PhysicsSystem> m_PhysicsSystem;
    std::unique_ptr<DebugPhysicsRenderer> m_DebugPhysicsRender;
};