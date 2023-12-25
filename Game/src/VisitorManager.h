#pragma once
#include "Entity.h"
#include "Visitor.h"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <type_traits>

class VisitorManager
{
public:
	template<typename T>
	bool IsVisitorRegistered() 
	{
		const char* typeName = typeid(T).name();
		return mVisitor.find(typeName) != mVisitor.end();
	}

	template<typename T>
	std::shared_ptr<T> GetVisitor() 
	{
		static_assert(std::is_base_of<VisitorBase, T>::value, "T must derive from visitor");
		const char* typeName = typeid(T).name();
		assert(mVisitor.find(typeName) != mVisitor.end() && "Getting System that does not exist");
		return std::dynamic_pointer_cast<T>(mVisitor[typeName]);
	}

	template<typename T>
	std::shared_ptr<T> RegisterVisitor()
	{
		static_assert(std::is_base_of<VisitorBase, T>::value, "T must derive from visitor");

		const char* typeName = typeid(T).name();

		assert(mVisitor.find(typeName) == mVisitor.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();

		mVisitor.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mVisitor.find(typeName) != mVisitor.end() && "System used before registered.");

		// Set the signature for this system
		mSignatures.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : mVisitor)
		{
			auto const& system = pair.second;

			system->m_Entities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : mVisitor)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			// Entity signature matches system signature - insert into set
			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->m_Entities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else
			{
				system->m_Entities.erase(entity);
			}
		}
	}


private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> mSignatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<VisitorBase>> mVisitor{};
};

