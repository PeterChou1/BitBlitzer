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

		return v->mEntities;
	}

};