#pragma once
#include "Entity.h"
#include "ComponentManager.h"
#include <set>
#include <memory>


class VisitorBase
{
public:
    std::set<Entity> m_Entities;
    virtual Signature GetRequirements(std::shared_ptr<ComponentManager> manager) = 0;
};


template <typename... Ts>
class Visitor : public VisitorBase
{
public:
    Signature GetRequirements(std::shared_ptr<ComponentManager> manager)
    {
        return manager->GetSignature<Ts...>();
    }
};
