#pragma once
#include <memory>

#include "GameState.h"


class WinConditionChecker
{
public:

    void Setup();

    void Update() const;

private:
    std::shared_ptr<GameState> m_GameState;

};
