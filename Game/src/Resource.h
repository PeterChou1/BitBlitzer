//---------------------------------------------------------------------------------
// Resources.h
//---------------------------------------------------------------------------------
// 
// Resources are global class that are initialized only once during
// the game startup in the ECS system they can optionally reset every the ECS system
// is cleared
//
#pragma once


class Resource
{
public:
    virtual ~Resource() = default;
    virtual void ResetResource() = 0;
};

