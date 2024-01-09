#pragma once
#include "stdafx.h"

#include <memory>

#include "Clipper.h"
#include "DebugCamera.h"
#include "DebugMesh.h"
#include "FragmentShader.h"
#include "MeshHandler.h"
#include "Rasterizer.h"
#include "VertexShader.h"

/**
 * \brief The game manager is responsible for managing all game scenes
 *        loading/unloading scene as well as handling the rendering pipeline
 */
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
};