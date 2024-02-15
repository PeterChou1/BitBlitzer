#pragma once

enum Owner
{
    Player1,
    Player2
};

struct PlayerOwnedCow
{
    Owner owner = Player1;

    PlayerOwnedCow() = default;
    PlayerOwnedCow(Owner owner) : owner(owner) {}

};