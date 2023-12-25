#pragma once
#include <set>
#include "Visitor.h"
#include "Coordinator.h"


class System {
public:
	template<typename ...Ts>
	std::set<Entity> Visit(Coordinator c) 
	{

		std::shared_ptr<Visitor<Ts...>> v;

		if (c.IsVisitorRegistered<Visitor<Ts...>>()) {
			v = c.GetVisitor<Visitor<Ts...>>();
		}
		else {
			v = c.RegisterVisitor<Visitor<Ts...>>();
		}
		return v->m_Entities;
	}
	/*
	 /brief Get First Component Registered with an Entity useful for getting component we know to be singletons like Cameras
     */
	template<typename T>
	T& GetFirstComponent(Coordinator c)
	{
		std::set<Entity> visitSet = Visit<T>(c);
		assert(visitSet.size() != 0, "No Entity with component found!");
		return c.GetComponent<T>(*visitSet.begin());
	}

};