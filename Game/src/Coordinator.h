#pragma once
#include "ComponentManager.h"
#include "EntityManager.h"
#include "VisitorManager.h"
#include "Entity.h"
#include <memory>
#include <set>

class Coordinator
{
public:
    void Init()
    {
        // Create pointers to each manager
        m_ComponentManager = std::make_shared<ComponentManager>();
        m_EntityManager = std::make_shared<EntityManager>();
        m_VisitorManager = std::make_shared<VisitorManager>();
    }


    // Entity methods
    Entity CreateEntity()
    {
        return m_EntityManager->CreateEntity();
    }


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
     * /brief Get First Component Registered with an Entity useful for getting component we know to be singletons like Cameras
     */
    template <typename T>
    T& GetFirstComponent()
    {
        std::set<Entity> visitSet = Visit<T>();
        assert(visitSet.size() != 0 && "No Entity with component found!");
        return GetComponent<T>(*visitSet.begin());
    }


    void DestroyEntity(Entity entity)
    {
        m_EntityManager->DestroyEntity(entity);

        m_ComponentManager->EntityDestroyed(entity);

        m_VisitorManager->EntityDestroyed(entity);
    }

    template <typename T>
    bool IsVisitorRegistered()
    {
        return m_VisitorManager->IsVisitorRegistered<T>();
    }

    template <typename T>
    std::shared_ptr<T> GetVisitor()
    {
        return m_VisitorManager->GetVisitor<T>();
    }

    // Component methods
    template <typename T>
    void RegisterComponent()
    {
        m_ComponentManager->RegisterComponent<T>();
    }

    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        m_ComponentManager->AddComponent<T>(entity, component);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);

        m_VisitorManager->EntitySignatureChanged(entity, signature);
    }


    template <typename T>
    void RemoveComponent(Entity entity)
    {
        m_ComponentManager->RemoveComponent<T>(entity);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);

        m_VisitorManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    T& GetComponent(Entity entity)
    {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        return m_ComponentManager->GetComponentType<T>();
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

    template <typename... Ts>
    Signature GetSignature()
    {
        return m_ComponentManager->GetSignature<Ts>();
    }

private:
    std::shared_ptr<ComponentManager> m_ComponentManager;
    std::shared_ptr<EntityManager> m_EntityManager;
    std::shared_ptr<VisitorManager> m_VisitorManager;
};
