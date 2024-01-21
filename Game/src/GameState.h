#pragma once

#include "Resource.h"
#include "Entity.h"

enum PlayState
{
    FreeRoamMode,
    CameraLerpToPosition,
    CameraLerpToEntity,
    LaunchMode,
    FollowProjectileMode,
    WinnerMode
};


enum PlayerTurn
{
    Player1Turn,
    Player2Turn,
    Player1Win,
    Player2Win,
    GameFinished
};

struct GameState : Resource
{
    PlayState State = FreeRoamMode;
    PlayerTurn Turn = Player1Turn;
    Entity Projectile = NULL_ENTITY;
    float MaxPower = 800.0f;
    int NumberOfCowsPlayer1 = 0;
    int NumberOfCowsPlayer2 = 0;

    void ResetResource() override
    {
        State = FreeRoamMode;
        Turn = Player1Turn;
        Projectile = NULL_ENTITY;
        NumberOfCowsPlayer1 = 0;
        NumberOfCowsPlayer2 = 0;
    }
};
