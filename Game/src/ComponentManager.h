#pragma once

#include "TypeID.h"
#include "ComponentBuffer.h"
#include "Entity.h"
#include <memory>
#include <unordered_map>



class ComponentManager
{
public:
    template <typename T>
    void RegisterComponent()
    {

        ComponentTypeID typeID = TypeID<T>::VALUE;

        assert(m_ComponentTypes.find(typeID) == m_ComponentTypes.end() && "Registering component type more than once.");
        assert(m_NextComponentType < MAX_COMPONENTS && "Maximum allowed registered components exceeded");

        m_ComponentTypes.insert({ typeID, m_NextComponentType});
        m_ComponentArrays.insert({ typeID, std::make_shared<ComponentBuffer<T>>()});

        ++m_NextComponentType;
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        if (m_ComponentTypes.find(typeID) == m_ComponentTypes.end())
            RegisterComponent<T>();
        return m_ComponentTypes[typeID];
    }

    template <typename... Ts>
    Signature GetSignature()
    {
        std::vector<ComponentType> types = {GetComponentType<Ts>()...};
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
        if (m_ComponentTypes.find(typeID) == m_ComponentTypes.end())
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
        if (m_ComponentTypes.find(typeID) != m_ComponentTypes.end())
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
        m_ComponentTypes.clear();
        m_ComponentArrays.clear();
        m_NextComponentType = 0;
    }

private:

    std::unordered_map<ComponentTypeID, ComponentType> m_ComponentTypes{};
    std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentBuffer>> m_ComponentArrays{};
    ComponentType m_NextComponentType{};


    template <typename T>
    std::shared_ptr<ComponentBuffer<T>> GetComponentArray()
    {
        ComponentTypeID typeID = TypeID<T>::VALUE;
        assert(m_ComponentTypes.find(typeID) != m_ComponentTypes.end() && "Component does not exist");
        return std::static_pointer_cast<ComponentBuffer<T>>(m_ComponentArrays[typeID]);
    }
};
