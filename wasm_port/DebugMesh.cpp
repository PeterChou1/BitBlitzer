#include "stdafx.h"
#include "DebugMesh.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "RigidBody.h"
#include "Transform.h"
#include "app.h"

extern ECSManager ECS;

void DebugMesh::Update(float deltaTime)
{

    std::vector<Entity> entities;

    for (auto e : ECS.Visit<Mesh, Transform>())
    {
        entities.push_back(e);
    }

    if (!entities.empty())
    {

        Entity last = entities.back();

        auto& transformFirst = ECS.GetComponent<Transform>(last);
        float speed = 1.0f * (deltaTime / 1000.0f);
        // if (App::IsKeyPressed('Z'))
        // {
        //     ECS.DestroyEntity(last);
        // }
        // 
        // 
        // if (App::IsKeyPressed('R'))
        // {
        //     Quat rot = Quat(Vec3(0, 0, 1.0), 1.0f * (deltaTime / 1000.0f));
        //     transformFirst.Update({0.0, 0.0, 0.0}, rot);
        // }
        // 
        // if (App::IsKeyPressed('Y'))
        // {
        //     Quat rot = Quat(Vec3(0, 0, 1.0), -1.0f * (deltaTime / 1000.0f));
        //     transformFirst.Update({ 0.0, 0.0, 0.0 }, rot);
        // }
        // 
        // if (App::IsKeyPressed('H'))
        // {
        //     // move first entity -x
        //     transformFirst.Update({-speed, 0.0, 0.0}, Quat());
        // }
        // 
        // if (App::IsKeyPressed('G'))
        // {
        //     // move first entity -y
        //     transformFirst.Update({ 0.0, -speed, 0.0 }, Quat());
        // }
        // 
        // if (App::IsKeyPressed('F'))
        // {
        //     // move first entity +x
        //     transformFirst.Update({ speed, 0.0, 0.0 }, Quat());
        // }
        // 
        // if (App::IsKeyPressed('T'))
        // {
        //     // move first entity +y
        //     transformFirst.Update({ 0.0, speed, 0.0 }, Quat());
        // }
    }



}
