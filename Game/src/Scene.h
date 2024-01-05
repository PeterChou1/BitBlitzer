#pragma once
#include "Coordinator.h"
#include "RendererSimple.h"
#include "DebugCamera.h"
#include "DebugMesh.h"
#include "RendererAVX.h"

class Scene
{
public:
    Scene() = default;

    void Setup();

    void Render();

    void Update(float deltaTime);

private:

    std::shared_ptr<RendererSimple> renderer;
    std::shared_ptr<RendererAVX> rendererM;
    std::shared_ptr<DebugCamera> debugCam;
    std::shared_ptr<DebugMesh> debugMesh;

};
