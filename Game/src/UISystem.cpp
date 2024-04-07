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

    if (m_GameState->State == BuildMode || m_GameState->State == FreeRoamMode)
    {
        if (m_GameState->State == BuildMode)
            PlayerTurn += "Preparation: ";
        else if (m_GameState->State == FreeRoamMode)
            PlayerTurn += "Turn: ";
        if (m_GameState->Turn == Player1Turn)
            PlayerTurn += "Player 1";
        else if (m_GameState->Turn == Player2Turn)
            PlayerTurn += "Player 2";

        App::Print(450, 700, PlayerTurn.c_str(), 0, 0, 0);
    }


    if (m_GameState->State == BuildMode)
    {
        std::string wood = "Press 1: To Buy Wood (50$)";
        std::string metal = "Press 2: To Buy Iron (100$)";
        std::string regularCow = "Press 3: To Buy Projectile Cow (50$)";
        std::string explosiveCow = "Press 4: To Buy Explosive Cow (200$)";

        App::Print(100, 700, wood.c_str(), 0, 0, 0);
        App::Print(100, 680, metal.c_str(), 0, 0, 0);
        App::Print(100, 660, regularCow.c_str(), 0, 0, 0);
        App::Print(100, 640, explosiveCow.c_str(), 0, 0, 0);

        std::string PlayerMoney;

        if (m_GameState->Turn == Player1Turn)
            PlayerMoney = "Money: " + std::to_string(m_GameState->Player1Money) + "$";

        if (m_GameState->Turn == Player2Turn)
            PlayerMoney = "Money: " + std::to_string(m_GameState->Player2Money) + "$";

        App::Print(750, 700, PlayerMoney.c_str(), 0, 0, 0);
        std::string endTurn = "Press Space To End Preparation";
        App::Print(450, 680, endTurn.c_str(), 1, 0, 0);

        PlayerInventory inventory = m_GameState->Player1Inventory;
        if (m_GameState->Turn == Player2Turn)
            inventory = m_GameState->Player2Inventory;

        std::string regularCowCount = "Regular Cow: " + std::to_string(inventory.regularCow);
        App::Print(750, 680, regularCowCount.c_str(), 0, 0, 0);
        std::string explosiveCowCount = "Explosive Cow: " + std::to_string(inventory.explosiveCow);
        App::Print(750, 660, explosiveCowCount.c_str(), 0, 0, 0);

    }
    else if (m_GameState->State == FreeRoamMode)
    {
        std::string PanInstruction = "WASD to pan ";
        std::string ZoomInstruction = "QE to Zoom Out/In";
        std::string LaunchProjectileInstruction = "Press Space To Aim Projectile";
        std::string Player1Health = "Cow's Alive (p1): " + std::string(m_GameState->NumberOfCowsPlayer1, 'o');
        std::string Player2Health = "Cow's Alive (p2): " + std::string(m_GameState->NumberOfCowsPlayer2, 'o');

        App::Print(150, 700, Player1Health.c_str(), 0, 0, 0);
        App::Print(750, 700, Player2Health.c_str(), 0, 0, 0);
        App::Print(450, 680, PanInstruction.c_str(), 1, 0, 0);
        App::Print(440, 660, ZoomInstruction.c_str(), 1, 0, 0);
        App::Print(400, 640, LaunchProjectileInstruction.c_str(), 1, 0, 0);

    }
    else if (m_GameState->State == LaunchMode)
    {
        std::string AimInstruction = "Use Your Mouse To Aim!";
        std::string LaunchInstruction = "Press Space To Launch Your Cow";
        std::string RegularInstruction = "Press 1 to Select Regular Cow";
        std::string ExplosiveInstruction = "Press 2 to Select Explosive Cow";

        App::Print(420, 680, AimInstruction.c_str(), 1, 0, 0);
        App::Print(400, 660, LaunchInstruction.c_str(), 1, 0, 0);
        App::Print(400, 640, RegularInstruction.c_str(), 1, 0, 0);
        App::Print(400, 620, ExplosiveInstruction.c_str(), 1, 0, 0);

        PlayerInventory inventory = m_GameState->Player1Inventory;
        if (m_GameState->Turn == Player2Turn)
            inventory = m_GameState->Player2Inventory;

        std::string regularCowCount = "Regular Cow: " + std::to_string(inventory.regularCow);


        if (inventory.regularCow == 0)
            App::Print(150, 600, regularCowCount.c_str(), 0.1f, 0.1f, 0.1f);
        else if (m_GameState->Selected == RegularSelect)
            App::Print(150, 600, regularCowCount.c_str(), 1, 0, 0);
        else
            App::Print(150, 600, regularCowCount.c_str(), 0, 0, 1);

        std::string explosiveCowCount = "Explosive Cow: " + std::to_string(inventory.explosiveCow);

        if (inventory.explosiveCow == 0)
            App::Print(750, 600, explosiveCowCount.c_str(), 0.1f, 0.1f, 0.1f);
        else if (m_GameState->Selected == ExplosiveSelect)
            App::Print(750, 600, explosiveCowCount.c_str(), 1, 0, 0);
        else
            App::Print(750, 600, explosiveCowCount.c_str(), 0, 0, 1);

    }
    else if (m_GameState->State == WinnerMode)
    {
        std::string WinText = "";
        std::string NewGameText = "Press Space To Replay";
    
        if (m_GameState->NumberOfCowsPlayer2 > m_GameState->NumberOfCowsPlayer1)
            WinText = "Player 2 Wins";
    
        if (m_GameState->NumberOfCowsPlayer1 > m_GameState->NumberOfCowsPlayer2)
            WinText = "Player 1 Wins";

        App::Print(460, 700, WinText.c_str(), 1, 1, 0);
        App::Print(430, 660, NewGameText.c_str(), 1, 1, 0);
    }
    else if (m_GameState->State == DrawMode)
    {
        std::string DrawGameText = "Draw Game Both Players Ran Out of Projectile";
        std::string NewGameText = "Press Space to replay";
        App::Print(350, 700, DrawGameText.c_str(), 1, 1, 0);
        App::Print(430, 660, NewGameText.c_str(), 1, 1, 0);
    }
}
