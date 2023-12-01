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
		mComponentManager = std::make_shared<ComponentManager>();
		mEntityManager = std::make_shared<EntityManager>();
		mVisitorManager = std::make_shared<VisitorManager>();
	}


	// Entity methods
	Entity CreateEntity()
	{
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mVisitorManager->EntityDestroyed(entity);
	}

	template<typename T>
	bool IsVisitorRegistered()
	{
		return mVisitorManager->IsSystemRegistered<T>();
	}

	template<typename T>
	std::shared_ptr<T> GetVisitor() 
	{
		return mVisitorManager->GetVisitor<T>();
	}
	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mVisitorManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		mVisitorManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return mComponentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterVisitor()
	{
		std::shared_ptr<T> visitor = mVisitorManager->RegisterVisitor<T>();
		std::shared_ptr<VisitorBase> visitorBase = visitor;
		Signature s = visitorBase->GetRequirements(mComponentManager);
		mVisitorManager->SetSignature<T>(s);
		visitorBase->mEntities = mEntityManager->MatchSignature(s);
		return visitor;
	}

	template<typename ...Ts>
	Signature GetSignature() {
		return mComponentManager->GetSignature<Ts>();
	}


private:
	std::shared_ptr<ComponentManager> mComponentManager;
	std::shared_ptr<EntityManager> mEntityManager;
	std::shared_ptr<VisitorManager> mVisitorManager;
};