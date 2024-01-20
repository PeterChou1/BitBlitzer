#pragma once
#include <unordered_map>
#include <vector>

#include "ColliderCategory.h"
#include "Manifolds.h"
#include "Resource.h"
#include "RigidBody.h"

class ColliderCallbackSystem : public Resource
{
public:

    ColliderCallbackSystem() = default;
    
    void RegisterCallback(const std::shared_ptr<Collider>& callback);

    void ResetResource() override;

    bool HasRegisterCallback(CollisionPair pair);

    void SubmitForCallback(Entity A, Entity B);

    void Update();

private:
    std::set<std::pair<Entity, Entity>> m_CollidePairs;
    std::unordered_map<CollisionPair, std::shared_ptr<Collider>, CollisionPairHash> m_CallBackMap;
};
