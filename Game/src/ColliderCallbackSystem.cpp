#include "stdafx.h"
#include "ColliderCallbackSystem.h"

extern ECSManager ECS;

void ColliderCallbackSystem::RegisterCallback(const std::shared_ptr<Collider>& callback)
{
    m_CallBackMap[callback->GetCollisionPair()] = callback;
}

void ColliderCallbackSystem::ResetResource()
{
    m_CallBackMap.clear();
}

bool ColliderCallbackSystem::HasRegisterCallback(CollisionPair pair)
{
    if (m_CallBackMap.find(pair) != m_CallBackMap.end())
        return true;

    std::swap(pair.first, pair.second);

    return m_CallBackMap.find(pair) != m_CallBackMap.end();
}

void ColliderCallbackSystem::SubmitForCallback(Entity A, Entity B)
{
    if (m_CollidePairs.find({A, B}) != m_CollidePairs.end() ||
        m_CollidePairs.find({B, A}) != m_CollidePairs.end())
        return;

    m_CollidePairs.insert({A, B});
}


void ColliderCallbackSystem::Update()
{
    std::set<Entity> deleteEntity = ECS.VisitDeleted<RigidBody>();
    
    for (auto& entityPair : m_CollidePairs)
    {
        Entity e1 = entityPair.first;
        Entity e2 = entityPair.second;
        if (deleteEntity.find(e1) != deleteEntity.end() ||
            deleteEntity.find(e2) != deleteEntity.end())
            continue;

        assert(ECS.HasComponent<RigidBody>(e1) && "Not Possible");
        assert(ECS.HasComponent<RigidBody>(e2) && "Not Possible");

        RigidBody& A = ECS.GetComponent<RigidBody>(e1);
        RigidBody& B = ECS.GetComponent<RigidBody>(e2);

        CollisionPair pairA = { A.Category, B.Category};
        CollisionPair pairB = { B.Category, A.Category};


        if (m_CallBackMap.find(pairA) != m_CallBackMap.end())
        {
            m_CallBackMap[pairA]->OnCollide(e1, e2,A,B);
        }
        else if (m_CallBackMap.find(pairB) != m_CallBackMap.end())
        {
            m_CallBackMap[pairB]->OnCollide(e2, e1, B, A);
        }
    
        deleteEntity = ECS.VisitDeleted<RigidBody>();
    }
    // Resolve all collider callback clear for next loop
    m_CollidePairs.clear();
}
