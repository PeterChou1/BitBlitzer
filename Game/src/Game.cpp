//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
//------------------------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------
#include "../app/app.h"
#include "Renderer.h"
#include "GameManager.h"
//------------------------------------------------------------------------

// RendererCPU renderer;
// Coordinator gCoordinator;
// Renderer rendererSys;

Coordinator gCoordinator;
Scene manager;

constexpr int imageSize = APP_VIRTUAL_WIDTH * APP_VIRTUAL_HEIGHT * 3;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
    App::SetupGL();
    gCoordinator.Init();
    manager.Setup();
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
    manager.Update(deltaTime);
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
    manager.Render();
}

//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
}
