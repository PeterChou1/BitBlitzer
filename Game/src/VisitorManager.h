//---------------------------------------------------------------------------------
// VisitorManager.h
//---------------------------------------------------------------------------------
//
// Manages All Visitor In the ECS System See Visitor.h for more information
// This class maps every Visitor to an Entity Signature that it is keeping track of
//
#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <type_traits>

#include "Entity.h"
#include "Visitor.h"

class VisitorManager
{
public:
    template <typename T>
    bool IsVisitorRegistered()
    {

        const char* typeName = typeid(T).name();

        return m_Visitor.find(typeName) != m_Visitor.end();
    }

    template <typename T>
    std::shared_ptr<T> GetVisitor()
    {
        static_assert(std::is_base_of<VisitorBase, T>::value, "T must derive from visitor");
        const char* typeName = typeid(T).name();
        assert(m_Visitor.find(typeName) != m_Visitor.end() && "Getting System that does not exist");
        return std::dynamic_pointer_cast<T>(m_Visitor[typeName]);
    }

    template <typename T>
    std::shared_ptr<T> RegisterVisitor()
    {
        static_assert(std::is_base_of<VisitorBase, T>::value, "T must derive from visitor");

        const char* typeName = typeid(T).name();

        assert(m_Visitor.find(typeName) == m_Visitor.end() && "Registering system more than once.");

        auto visitor = std::make_shared<T>();

        m_Visitor.insert({typeName, visitor });
        return visitor;
    }

    template <typename T>
    void SetSignature(Signature signature)
    {
        const char* typeName = typeid(T).name();

        assert(m_Visitor.find(typeName) != m_Visitor.end() && "System used before registered.");

        // Set the signature for this system
        m_Signatures.insert({typeName, signature});
    }

    void EntityDestroyed(Entity entity)
    {
        for (auto const& pair : m_Visitor)
        {
            auto const& visitor = pair.second;
            visitor->m_Entities.erase(entity);
        }
    }

    void EntitySignatureDeleted(Entity entity, Signature entitySignature)
    {
        for (auto const& pair : m_Visitor)
        {
            auto const& type = pair.first;
            auto const& visitor = pair.second;
            auto const& systemSignature = m_Signatures[type];

            if ((entitySignature & systemSignature) == systemSignature)
            {
                visitor->m_DeletedEntities.insert(entity);
            }
        }
    }

    void FlushDeletedEntities()
    {
        for (auto const& pair : m_Visitor)
        {
            auto const& visitor = pair.second;
            visitor->m_DeletedEntities.clear();
        }
    }

    void EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        for (auto const& pair : m_Visitor)
        {
            auto const& type = pair.first;
            auto const& visitor = pair.second;
            auto const& systemSignature = m_Signatures[type];

            if ((entitySignature & systemSignature) == systemSignature)
            {
                visitor->m_Entities.insert(entity);
            }
            else
            {
                visitor->m_Entities.erase(entity);
            }
        }
    }


    void Clear()
    {
        m_Signatures.clear();
        m_Visitor.clear();
    }

private:
    // Map from type string pointer to a signature
    std::unordered_map<const char*, Signature> m_Signatures{};

    // Map from type string pointer to a Visitor
    std::unordered_map<const char*, std::shared_ptr<VisitorBase>> m_Visitor{};
};
