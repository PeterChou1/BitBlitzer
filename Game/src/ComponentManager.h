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

        ComponentTypeID typeID = TypeID<T>::value;

        assert(mComponentTypes.find(typeID) == mComponentTypes.end() && "Registering component type more than once.");
        assert(mNextComponentType < MAX_COMPONENTS && "Maximum allowed registered components exceeded");

        mComponentTypes.insert({ typeID, mNextComponentType});
        mComponentArrays.insert({ typeID, std::make_shared<ComponentBuffer<T>>()});

        ++mNextComponentType;
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        ComponentTypeID typeID = TypeID<T>::value;
        if (mComponentTypes.find(typeID) == mComponentTypes.end())
        {
            // component registerd before use
            RegisterComponent<T>();
        }
        return mComponentTypes[typeID];
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
        ComponentTypeID typeID = TypeID<T>::value;
        if (mComponentTypes.find(typeID) == mComponentTypes.end())
        {
            // component registerd before use
            RegisterComponent<T>();
        }
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template <typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity)
    {
        for (auto const& pair : mComponentArrays)
        {
            auto const& component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

    void Clear()
    {
        mComponentTypes.clear();
        mComponentArrays.clear();
        mNextComponentType = 0;
    }

private:
    // maps type name -> type id 
    std::unordered_map<ComponentTypeID, ComponentType> mComponentTypes{};

    // maps type 
    std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentBuffer>> mComponentArrays{};
    ComponentType mNextComponentType{};


    template <typename T>
    std::shared_ptr<ComponentBuffer<T>> GetComponentArray()
    {
        ComponentTypeID typeID = TypeID<T>::value;
        assert(mComponentTypes.find(typeID) != mComponentTypes.end() && "Component does not exist");
        return std::static_pointer_cast<ComponentBuffer<T>>(mComponentArrays[typeID]);
    }
};
