//---------------------------------------------------------------------------------
// ComponentBuffer.h
//---------------------------------------------------------------------------------
// 
//  Component Buffers are the core of the ECS implementation they store
//  meant to store components in a contiguous array for fast cache access
//


#pragma once

#include <array>
#include <cassert>
#include <unordered_map>
#include "Entity.h"



class IComponentBuffer
{
public:
    virtual ~IComponentBuffer() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};


template <typename T>
class ComponentBuffer : public IComponentBuffer
{
public:
    ComponentBuffer()
    {
    }

    void InsertData(Entity entity, T component)
    {
        assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end() && "Component added to same entity more than once.");

        // Put new entry at end
        size_t newIndex = m_Size;
        m_EntityToIndexMap[entity] = newIndex;
        m_IndexToEntityMap[newIndex] = entity;
        m_ComponentArray[newIndex] = component;
        ++m_Size;
    }

    void RemoveData(Entity entity)
    {
        assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
        size_t indexOfLastElement = m_Size - 1;
        m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
        m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        m_EntityToIndexMap.erase(entity);
        m_IndexToEntityMap.erase(indexOfLastElement);

        --m_Size;
    }

    bool HasData(Entity entity)
    {
        return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
    }

    T& GetData(Entity entity)
    {
        assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Retrieving non-existent component.");

        return m_ComponentArray[m_EntityToIndexMap[entity]];
    }

    void EntityDestroyed(Entity entity) override
    {
        if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end())
        {
            RemoveData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> m_ComponentArray{};
    std::unordered_map<Entity, size_t> m_EntityToIndexMap{};
    std::unordered_map<size_t, Entity> m_IndexToEntityMap{};
    size_t m_Size{};
};
