#pragma once
#include "AABB.h"
#include "ECSManager.h"

typedef void (*ColliderCallback) (ECSManager& ECS, Entity e);

/**
 * \brief A Collider Detects 
 */
class Collider
{
    Collider(
        float width,
        float height,
        ColliderCallback callback
    ) : m_Callback(callback)
    {
        
    }

    ColliderCallback m_Callback;
    AABB aabb;
};