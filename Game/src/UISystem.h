#pragma once
#include <memory>
#include "GameState.h"


class UISystem
{
public:

    void Setup();

    void Render() const;

private:
    std::shared_ptr<GameState> m_GameState;
};
