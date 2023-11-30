#pragma once
#include "Entity.h"
#include "ComponentManager.h"
#include <set>
#include <vector>
#include <memory>



class System
{
public:
	std::set<Entity> mEntities;

	virtual void Update(float delta) = 0;

	virtual void Render() = 0;

	virtual std::vector<const char*> GetRequirements() = 0;
};


