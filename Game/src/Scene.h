#pragma once
#include "ECSManager.h"
#include "Renderer.h"
#include "DebugCamera.h"
#include "DebugMesh.h"

class Scene
{
public:
    Scene() = default;

    void Setup();

    void Render();

    void Update(float deltaTime);

private:
    std::shared_ptr<Renderer> rendererM;
    std::shared_ptr<DebugCamera> debugCam;
    std::shared_ptr<DebugMesh> debugMesh;
};
