#include "stdafx.h"
#include "UISystem.h"

#include <string>

#include "ECSManager.h"
#include "../App/app.h"

extern ECSManager ECS;

void UISystem::Setup()
{
    m_GameState = ECS.GetResource<GameState>();
}

void UISystem::Render() const
{

    std::string PlayerTurn = "";

    if (m_GameState->Turn == Player1Turn)
        PlayerTurn = "Player 1 Turn";
    
    if (m_GameState->Turn == Player2Turn)
        PlayerTurn = "Player 2 Turn";


    std::string Player1Health = "Cow's Alive (p1): " + std::to_string(m_GameState->NumberOfCowsPlayer1);
    std::string Player2Health = "Cow's Alive (p2): " + std::to_string(m_GameState->NumberOfCowsPlayer2);

    App::Print(100, 700, PlayerTurn.c_str());
    App::Print(100, 680, Player1Health.c_str());
    App::Print(100, 660, Player2Health.c_str());

    if (m_GameState->State == FreeRoamMode)
    {
        std::string PanInstruction = "WASD to pan ";
        std::string ZoomInstruction = "QE to Zoom Out/In";
        std::string LaunchProjectileInstruction = "Press Space To Aim Projectile";

        App::Print(600, 700, PanInstruction.c_str());
        App::Print(600, 680, ZoomInstruction.c_str());
        App::Print(600, 660, LaunchProjectileInstruction.c_str());
    }

    if (m_GameState->State == LaunchMode)
    {
        std::string AimInstruction = "Use Your Mouse To Aim!";
        std::string LaunchInstruction = "Press Space To Launch Your Cow";

        App::Print(600, 700, AimInstruction.c_str());
        App::Print(600, 680, LaunchInstruction.c_str());
    }

    if (m_GameState->State == WinnerMode)
    {
        float height = APP_VIRTUAL_HEIGHT / 2;
        float width = APP_VIRTUAL_WIDTH / 2;

        std::string WinText = "";

        std::string NewGameText = "Press Space to replay";

        if (m_GameState->NumberOfCowsPlayer1 == 0)
            WinText = "Player 2 Wins";

        if (m_GameState->NumberOfCowsPlayer2 == 0)
            WinText = "Player 1 Wins";

        App::Print(width, height, WinText.c_str());
        App::Print(width, height - 50, NewGameText.c_str());

    }
}
