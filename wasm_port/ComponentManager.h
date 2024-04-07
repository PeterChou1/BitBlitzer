//---------------------------------------------------------------------------------
// ComponentManager.h
//---------------------------------------------------------------------------------
// 
// Manages ComponentBuffers during the game session the class
// is simple Storage class that maps a ComponentBuffer to their ComponentTypeID
//

#pragma once

#include "TypeID.h"
#include "ComponentBuffer.h"
#include "Entity.h"
#include <memory>
#include <set>
#include <unordered_map>



class ComponentManager
{
public:
    template <typename T>
    void RegisterComponent()
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        assert(m_RegisteredComponents.find(typeID) == m_RegisteredComponents.end() && "Registering component type more than once.");
        assert(typeID < MAX_COMPONENTS && "Maximum allowed registered components exceeded");
        m_RegisteredComponents.insert(typeID);
        m_ComponentArrays.insert({ typeID, std::make_shared<ComponentBuffer<T>>()});
    }

    template <typename T>
    ComponentTypeID GetComponentType()
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        if (m_RegisteredComponents.find(typeID) == m_RegisteredComponents.end())
            RegisterComponent<T>();

        return typeID;
    }

    template <typename... Ts>
    Signature GetSignature()
    {
        std::vector<ComponentTypeID> types = {GetComponentType<Ts>()...};
        Signature s;
        for (auto cType : types)
        {
            s.set(cType);
        }
        return s;
    }

    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        if (m_RegisteredComponents.find(typeID) == m_RegisteredComponents.end())
            RegisterComponent<T>();
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template <typename T>
    bool HasComponent(Entity entity)
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        if (m_RegisteredComponents.find(typeID) != m_RegisteredComponents.end())
            return GetComponentArray<T>()->HasData(entity);
        return false;
    }

    template <typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity)
    {
        for (auto const& pair : m_ComponentArrays)
        {
            auto const& component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

    void Clear()
    {
        m_ComponentArrays.clear();
        m_RegisteredComponents.clear();
    }

private:

    std::set<ComponentTypeID> m_RegisteredComponents;
    std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentBuffer>> m_ComponentArrays{};

    template <typename T>
    std::shared_ptr<ComponentBuffer<T>> GetComponentArray()
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        assert(m_RegisteredComponents.find(typeID) != m_RegisteredComponents.end() && "Component does not exist");
        return std::static_pointer_cast<ComponentBuffer<T>>(m_ComponentArrays[typeID]);
    }
};
