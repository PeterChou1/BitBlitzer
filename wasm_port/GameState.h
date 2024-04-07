#pragma once

#include "Resource.h"
#include "Entity.h"

enum PlayState
{
    IntroMode,
    StartMode,
    BuildMode,
    FreeRoamMode,
    CameraLerpToPosition,
    CameraLerpToEntity,
    LaunchMode,
    FollowProjectileMode,
    WinnerMode,
    DrawMode
};


enum PlayerTurn
{
    Player1Turn,
    Player2Turn,
    Player1Win,
    Player2Win,
    Draw,
    GameFinished
};

enum CowSelected
{
    RegularSelect,
    ExplosiveSelect
};

struct PlayerInventory
{
    int regularCow{};
    int explosiveCow{};
};

struct GameState : Resource
{
    PlayState State = IntroMode;
    PlayerTurn Turn = Player1Turn;
    Entity Projectile = NULL_ENTITY;
    CowSelected Selected = RegularSelect;

    float MaxPower = 1500.0f;
    int NumberOfCowsPlayer1 = 0;
    int NumberOfCowsPlayer2 = 0;
    int Player1Money = 1000;
    int Player2Money = 1500;

    PlayerInventory Player1Inventory;
    PlayerInventory Player2Inventory;


    void ResetResource() override
    {
        State = IntroMode;
        Turn = Player1Turn;
        Selected = RegularSelect;
        Player1Inventory = PlayerInventory();
        Player2Inventory = PlayerInventory();
        Player1Money = 1000;
        Player2Money = 1500;
        Projectile = NULL_ENTITY;
        NumberOfCowsPlayer1 = 0;
        NumberOfCowsPlayer2 = 0;
    }
};
