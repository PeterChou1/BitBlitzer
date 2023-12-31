#pragma once
#include "Coordinator.h"
#include "Renderer.h"
#include "DebugCamera.h"
#include "RendererM.h"

class Scene
{
public:
    Scene()
    {
    }

    void Setup();

    void Render();

    void Update(float deltaTime);

private:
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<RendererM> rendererM;
    std::shared_ptr<DebugCamera> debugCam;
};
