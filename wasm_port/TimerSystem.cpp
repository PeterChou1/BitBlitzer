#include "stdafx.h"
#include "TimerSystem.h"
#include "ECSManager.h"
#include "Timer.h"

extern ECSManager ECS;

void TimerSystem::Update(float deltaTime)
{
    std::vector<Entity> deletedEntites;

    for (Entity e : ECS.Visit<Timer>())
    {
        Timer& timer = ECS.GetComponent<Timer>(e);

        timer.CurrentTime -= deltaTime / 1000.0f;

        if (timer.CurrentTime < 0.0f)
        {
            deletedEntites.push_back(e);
        }
    }

    for (Entity e : deletedEntites)
    {
        ECS.DestroyEntity(e);
    }
}
