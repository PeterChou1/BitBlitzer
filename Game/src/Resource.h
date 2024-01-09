#pragma once

/**
 * \brief Resources are global class that are initialized only once
 *        during the startup of the Game
 */
class Resource
{
public:
    virtual ~Resource() = default;
    virtual void ClearResource() = 0;
};