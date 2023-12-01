#pragma once

#include "ComponentArray.h"
#include "Entity.h"
#include <any>
#include <memory>
#include <unordered_map>


class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		mComponentTypes.insert({ typeName, mNextComponentType });
		mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		++mNextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();
		if (mComponentTypes.find(typeName) == mComponentTypes.end()) {
			// component registerd before use
			RegisterComponent<T>();
		}
		return mComponentTypes[typeName];
	}

	template<typename ...Ts>
	Signature GetSignature() {
		std::vector<ComponentType> types = { GetComponentType<Ts>()... };
		Signature s;
		for (auto cType : types) {
			s.set(cType);
		}
		return s;
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		const char* typeName = typeid(T).name();
		if (mComponentTypes.find(typeName) == mComponentTypes.end()) {
			// component registerd before use
			RegisterComponent<T>();
		}
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
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

private:
	// maps type name -> type id 
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// maps type 
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
	ComponentType mNextComponentType{};


	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();
		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component does not exist");
		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};
