//---------------------------------------------------------------------------------
// ECSManager.h
//---------------------------------------------------------------------------------
// 
// The ECSManager is the interface with which the Developer Interfaces
// with the ECS System it manages the global state of the game
// ECS stands for Entity Component System,
// Broadly speaking:
//   - Entities are containers for Components
//   - Components are data class
//   - Systems act on the Components
//
// The ECS System implementation currently allows for the addition
// of resources which are globally unique entities that are instantiate once
// per game 
//
#pragma once

#include <memory>
#include <set>

#include "ComponentManager.h"
#include "EntityManager.h"
#include "VisitorManager.h"
#include "Entity.h"
#include "Resource.h"

class ECSManager
{
public:

    /**
     * \brief Initialized all subcomponents of the ECS system
     */
    void Init()
    {
        // Create pointers to each manager
        m_ComponentManager = std::make_shared<ComponentManager>();
        m_EntityManager = std::make_shared<EntityManager>();
        m_VisitorManager = std::make_shared<VisitorManager>();
    }

    /**
     * \brief Meant for internal use flushes deleted Entity list
     */
    void FlushECS()
    {
        m_VisitorManager->FlushDeletedEntities();
    }

    /**
     * \brief Resets the ECS System by clearing everything
     */
    void Reset()
    {
        m_ComponentManager->Clear();
        m_EntityManager->Clear();
        m_VisitorManager->Clear();
        for (auto resource: m_Resources)
        {
             resource->ResetResource();
        }
    }


    /**
     * \brief Creates an Entity in the ECS System
     * \return an Entity ID
     */
    Entity CreateEntity()
    {
        return m_EntityManager->CreateEntity();
    }

    /**
     * \brief Visit all entities that hold a particular Ts Components
     * \tparam Ts Component Types the entities have
     * \return all Entity that hold Ts data types 
     */
    template <typename... Ts>
    std::set<Entity> Visit()
    {
        std::shared_ptr<Visitor<Ts...>> v;
        if (IsVisitorRegistered<Visitor<Ts...>>())
        {
            v = GetVisitor<Visitor<Ts...>>();
        }
        else
        {
            v = RegisterVisitor<Visitor<Ts...>>();
        }
        return v->m_Entities;
    }


    /**
     * \brief Visit all entities that hold a particular set of Entity
     *        that were deleted by other Systems in this game loop
     *
     * \tparam Ts Component Types to Visit
     * \return All Entity that hold Ts data types that were deleted by
     *         other systems
     */
    template <typename... Ts>
    std::set<Entity> VisitDeleted()
    {
        std::shared_ptr<Visitor<Ts...>> v;
        if (IsVisitorRegistered<Visitor<Ts...>>())
        {
            v = GetVisitor<Visitor<Ts...>>();
        }
        else
        {
            v = RegisterVisitor<Visitor<Ts...>>();
        }

        return v->m_DeletedEntities;
    }


    /**
     * \brief Remove the Entity from the ECS System
     */
    void DestroyEntity(Entity entity)
    {
        auto signature = m_EntityManager->GetSignature(entity);
        m_VisitorManager->EntitySignatureDeleted(entity, signature);
        m_EntityManager->DestroyEntity(entity);
        m_ComponentManager->EntityDestroyed(entity);
        m_VisitorManager->EntityDestroyed(entity);
    }


    /**
     * \brief Add Component T to Entity
     */
    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        m_ComponentManager->AddComponent<T>(entity, component);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);

        m_VisitorManager->EntitySignatureChanged(entity, signature);
    }

    /**
     * \brief Remove Component T from Entity
     */
    template <typename T>
    void RemoveComponent(Entity entity)
    {
        m_ComponentManager->RemoveComponent<T>(entity);
        auto signature = m_EntityManager->GetSignature(entity);
        m_VisitorManager->EntitySignatureDeleted(entity, signature);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);
        m_VisitorManager->EntitySignatureChanged(entity, signature);
    }

    /**
     * \brief Check if Entity has Component T
     */
    template <typename T>
    bool HasComponent(Entity entity) const
    {
        return m_ComponentManager->HasComponent<T>(entity);
    }

    /**
     * \brief GetComponent T from Entity
     */
    template <typename T>
    T& GetComponent(Entity entity)
    {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    /**
     * \brief Registered a Resource within the ECS System
     *        Note you cannot register a resource twice
     */
    template <typename T>
    void RegisterResource(T resource)
    {
        static_assert(std::is_base_of<Resource, T>::value, "T must derive from resource");
        const char* typeName = typeid(T).name();
        assert(m_ResourceToIndex.find(typeName) == m_ResourceToIndex.end() && "Resource already registered");
        m_ResourceToIndex[typeName] = m_Resources.size();
        m_Resources.push_back(std::make_shared<T>(resource));
    }

    /**
     * \brief Get a Registered Resource T
     */
    template <typename T>
    std::shared_ptr<T> GetResource()
    {
        static_assert(std::is_base_of<Resource, T>::value, "T must derive from resource");
        const char* typeName = typeid(T).name();
        assert(m_ResourceToIndex.find(typeName) != m_ResourceToIndex.end() && "Resource Not Registered");
        return std::dynamic_pointer_cast<T>(m_Resources[m_ResourceToIndex[typeName]]);
    }


    template <typename... Ts>
    Signature GetSignature()
    {
        return m_ComponentManager->GetSignature<Ts>();
    }

private:
    template <typename T>
    ComponentTypeID GetComponentType()
    {
        return m_ComponentManager->GetComponentType<T>();
    }


    template <typename T>
    std::shared_ptr<T> GetVisitor()
    {
        return m_VisitorManager->GetVisitor<T>();
    }

    template <typename T>
    bool IsVisitorRegistered() const
    {
        return m_VisitorManager->IsVisitorRegistered<T>();
    }

    // Component methods
    template <typename T>
    void RegisterComponent()
    {
        m_ComponentManager->RegisterComponent<T>();
    }

    // System methods
    template <typename T>
    std::shared_ptr<T> RegisterVisitor()
    {
        std::shared_ptr<T> visitor = m_VisitorManager->RegisterVisitor<T>();
        std::shared_ptr<VisitorBase> visitorBase = visitor;
        Signature s = visitorBase->GetRequirements(m_ComponentManager);
        m_VisitorManager->SetSignature<T>(s);
        visitorBase->m_Entities = m_EntityManager->MatchSignature(s);
        return visitor;
    }


    std::shared_ptr<ComponentManager> m_ComponentManager;
    std::shared_ptr<EntityManager> m_EntityManager;
    std::shared_ptr<VisitorManager> m_VisitorManager;
    std::unordered_map<const char*, size_t> m_ResourceToIndex;
    std::vector<std::shared_ptr<Resource>> m_Resources;
};
