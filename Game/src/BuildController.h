#pragma once
#include <memory>

#include "Cursors.h"
#include "GameState.h"

class BuildController
{
public:
    void Setup();

    void Update(float deltaTime);

    void Render();
private:


    void HandleBuild();

    void HandleSelect();

    float m_Accumulate = 0;
    float m_RotateAccumulate = 0;
    bool m_IsSelected = false;
    Entity m_Selected{};
    std::shared_ptr<Cursor> m_Cursor;
    std::shared_ptr<GameState> m_GameState;
};
