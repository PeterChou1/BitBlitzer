#pragma once
#include "Entity.h"
#include "Resource.h"


struct Cursor : Resource
{
    Entity CursorEntity = NULL_ENTITY;
    float ScreenX, ScreenY;
    float X, Y;

    void ResetResource() override
    {
        CursorEntity = NULL_ENTITY;
    }
};
