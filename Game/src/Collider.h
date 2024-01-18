//---------------------------------------------------------------------------------
// Collider.h
//---------------------------------------------------------------------------------
//
// (PLEASE FILL ME AFTER IMPLEMENTATION)
//

#pragma once
#include "ECSManager.h"

/**
 * \brief A Collider Detects 
 */
class Collider
{
    Collider() = default;

    virtual void OnEnter(ECSManager ecs, Entity e) = 0;

    virtual void OnExit(ECSManager ecs, Entity e) = 0;


};