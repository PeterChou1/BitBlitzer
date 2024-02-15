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
#include "Scene.h"
#include "TimerSystem.h"
#include "VertexShader.h"

class GameManager
{
public:

    /**
     * \brief  Set up the game instance as well as all scenes in the game
     */
    void Setup();

    /**
     * \brief Update loop runs current active scene as well as physics loop
     * \param deltaTime 
     */
    void Update(float deltaTime);

    /**
     * \brief Runs the Render pipeline
     */
    void Render();


    /**
     * \brief Registers a scene within the game
     */
    void RegisterScene(const std::string& sceneName, std::unique_ptr<Scene> scene);

    /**
     * \brief Resets the ECS and sets a new current scene
     * \param sceneName 
     */
    void SetActiveScene(const std::string& sceneName);

private:
    std::string m_ActiveScene;
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_SceneMap;

    std::unique_ptr<VertexShader> m_VertexShader;
    std::unique_ptr<Clipper> m_Clipper;
    std::unique_ptr<Rasterizer> m_Rasterizer;
    std::unique_ptr<FragmentShader> m_FragmentShader;
    std::unique_ptr<MeshHandler> m_MeshHandler;
    std::unique_ptr<DebugCamera> m_DebugCamera;
    std::unique_ptr<DebugMesh> m_DebugMesh;
    std::unique_ptr<PhysicsSystem> m_PhysicsSystem;
    std::unique_ptr<DebugPhysicsRenderer> m_DebugPhysicsRender;
    std::unique_ptr<TimerSystem> m_TimerSystem;
};