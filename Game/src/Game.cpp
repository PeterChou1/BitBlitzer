//------------------------------------------------------------------------
// Game.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <memory>

#include "ECSManager.h"
#include "GameManager.h"
#include "Level1.h"
#include "TitleScreen.h"
//------------------------------------------------------------------------


ECSManager ECS;
GameManager GameSceneManager;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
    // These must be initialized before any other systems
    ECS.Init();
    GameSceneManager.Setup();
    // Register all game scenes below
    std::unique_ptr<Scene> titleScreen = std::make_unique<TitleScreen>();
    std::unique_ptr<Scene> level1 = std::make_unique<Level1>();
    GameSceneManager.RegisterScene("Level1", std::move(level1));
    GameSceneManager.RegisterScene("TitleScreen", std::move(titleScreen));
    GameSceneManager.SetActiveScene("Level1");
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
    GameSceneManager.Update(deltaTime);
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
    GameSceneManager.Render();
    ECS.FlushECS();
}

//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
}
