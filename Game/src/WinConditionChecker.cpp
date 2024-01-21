#include "stdafx.h"

#include "WinConditionChecker.h"
#include "Cows.h"
#include "ECSManager.h"
#include "GameManager.h"
#include "../App/app.h"

extern ECSManager ECS;
extern GameManager GameSceneManager;

void WinConditionChecker::Setup()
{
    m_GameState = ECS.GetResource<GameState>();
}

void WinConditionChecker::Update() const
{
    if (m_GameState->State == WinnerMode && App::GetController().CheckButton(XINPUT_GAMEPAD_A))
    {
        GameSceneManager.SetActiveScene("Level1");
    }

    int player1OwnedCow = 0;
    int player2OwnedCow = 0;
    for (Entity e : ECS.Visit<PlayerOwnedCow>())
    {
        auto& owned = ECS.GetComponent<PlayerOwnedCow>(e);
        if (owned.owner == Player1)
        {
            player1OwnedCow += 1;
        }
        else if (owned.owner == Player2)
        {
            player2OwnedCow += 1;
        }
    }
    m_GameState->NumberOfCowsPlayer1 = player1OwnedCow;
    m_GameState->NumberOfCowsPlayer2 = player2OwnedCow;

    if (player1OwnedCow == 0 && m_GameState->Turn != GameFinished)
    {
        m_GameState->Turn = Player2Win;
    }

    if (player2OwnedCow == 0 && m_GameState->Turn != GameFinished)
    {
        m_GameState->Turn = Player1Win;
    }
}

