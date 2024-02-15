//---------------------------------------------------------------------------------
// Visitor.h
//---------------------------------------------------------------------------------
// 
// The Visitor Class represent the System of the Entity Component System
// Each Visitor contains a list of Entities in the ECS System
// that have certain components
//
#pragma once

#include <set>
#include <memory>

#include "Entity.h"
#include "ComponentManager.h"

class VisitorBase
{
public:
    std::set<Entity> m_Entities;
    std::set<Entity> m_DeletedEntities;
    virtual Signature GetRequirements(std::shared_ptr<ComponentManager> manager) = 0;
};

/**
 * \brief Keeps track of all Components with data types Ts
 * \tparam Ts 
 */
template <typename... Ts>
class Visitor : public VisitorBase
{
public:
    Signature GetRequirements(std::shared_ptr<ComponentManager> manager)
    {
        return manager->GetSignature<Ts...>();
    }
};
