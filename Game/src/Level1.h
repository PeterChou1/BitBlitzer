#pragma once
#include "CameraController.h"
#include "ColliderCallbackSystem.h"
#include "GameState.h"
#include "LaunchController.h"
#include "Lighting.h"
#include "Scene.h"
#include "UISystem.h"
#include "WinConditionChecker.h"


class Level1 : public Scene
{
public:

    Level1() = default;

    ~Level1() override = default;

    void Start() override;

    void Setup() override;

    void Update(float deltaTime) override;

    void Render() override;


private:
    /// I spent a lot of time writing the 3d render of course gonna use it :)
    void DecorateScene();

    std::shared_ptr<GameState> m_GameState;
    std::shared_ptr<ColliderCallbackSystem> m_ColliderCallback;
    std::shared_ptr<Lighting> m_Lights;
    std::unique_ptr<UISystem> m_UISystem;
    std::unique_ptr<CameraController> m_CamController;
    std::unique_ptr<LaunchController> m_LaunchController;
    std::unique_ptr<WinConditionChecker> m_WinConditionChecker;

};
