#include "stdafx.h"
#include "DebugMesh.h"
#include "Coordinator.h"
#include "Mesh.h"
#include "Transform.h"
#include "../App/app.h"

extern Coordinator ECS;

void DebugMesh::Update(float deltaTime)
{

    std::vector<Entity> entities;

    for (auto e : ECS.Visit<Mesh, Transform>())
    {
        entities.push_back(e);
    }

    if (!entities.empty())
    {

        Entity first = entities.front();
        Entity last = entities.back();

        auto& meshFirst = ECS.GetComponent<Mesh>(first);
        auto& meshLast = ECS.GetComponent<Mesh>(last);
        auto& transformFirst = ECS.GetComponent<Transform>(first);

        float speed = 5.0f * (deltaTime / 1000.0f);

        if (App::IsKeyPressed('Z'))
        {
            // delete the first
            // entities
            meshFirst.markedForDeletion = true;
            meshLast.markedForDeletion = true;
        }


        if (App::IsKeyPressed('4'))
        {
            // move first entity -x
            transformFirst.Update({-speed, 0.0, 0.0}, Quat());
        }

        if (App::IsKeyPressed('5'))
        {
            // move first entity -y
            transformFirst.Update({ 0.0, -speed, 0.0 }, Quat());
        }

        if (App::IsKeyPressed('6'))
        {
            // move first entity +x
            transformFirst.Update({ speed, 0.0, 0.0 }, Quat());
        }

        if (App::IsKeyPressed('8'))
        {
            // move first entity +y
            transformFirst.Update({ 0.0, speed, 0.0 }, Quat());
        }
    }



}
