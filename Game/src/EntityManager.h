//---------------------------------------------------------------------------------
// EntityManager.h
//---------------------------------------------------------------------------------
// 
// Manages All Entity in the ECS System
// An Entity Manager maps an Entity to their Signature
// which denotes which component they own
// 

#pragma once
#include <array>
#include <cassert>
#include <queue>
#include <set>
#include "Entity.h"

class EntityManager
{
public:
    EntityManager()
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            m_AvailableEntities.push(entity);
        }
    }

    Entity CreateEntity()
    {
        assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

        Entity id = m_AvailableEntities.front();
        m_AvailableEntities.pop();
        ++m_LivingEntityCount;

        return id;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        m_Signatures[entity].reset();
        m_AvailableEntities.push(entity);
        --m_LivingEntityCount;
    }

    void SetSignature(Entity entity, Signature signature)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        m_Signatures[entity] = signature;
    }

    Signature GetSignature(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        return m_Signatures[entity];
    }

    std::set<Entity> MatchSignature(Signature signature)
    {
        std::set<Entity> entities;

        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            if ((m_Signatures[entity] & signature) == signature)
            {
                entities.insert(entity);
            }
        }

        return entities;
    }

    void Clear()
    {
        //clear queue
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            m_AvailableEntities.push(entity);
        }
        for (Signature& signature : m_Signatures)
        {
            signature.reset();
        }
        m_LivingEntityCount = 0;
    }

private:
    std::queue<Entity> m_AvailableEntities{};
    std::array<Signature, MAX_ENTITIES> m_Signatures{};
    uint32_t m_LivingEntityCount{};
};
